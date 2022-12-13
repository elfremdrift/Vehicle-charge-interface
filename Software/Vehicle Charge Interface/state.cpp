// Charger state machine

#include "definitions.h"

enum class st : byte {
	  powerOn = 0
	, idle
	, cpOrPp
	, locking
	, locked
	, charging
	, noPower
	, unlocking
	, unlocked
	, noChange
	, nStates
};

const char stNames[][11] PROGMEM = {
	  "powerOn   "
	, "idle      "
	, "cpOrPp    "
	, "locking   "
	, "locked    "
	, "charging  "
	, "noPower   "
	, "unlocking "
	, "unlocked  "
	, "noChange  "
	, "nStates   "
};

enum class lamps : byte {
	none,
	red,
	redFlash,
	green,
	greenFlash,
	yellow,
	yellowFlash,
	all,
	allFlash
};

struct State {
	// Next state based on read values;
	const st		noCpOrPp;		  // Next state if neither CP nor PP present
	const st		cpOrPp;				// Next state if either CP or PP present
	const st		cpAndPp;			// Next state if both CP and PP present
	const st		lockLocked;		// Next state if lock indicates locked
	const st		lockUnlocked;	// Next state if lock indicates unlocked
	const st		unlockSwitch;	// Next state if unlock switch is pressed
	const st		timeout;			// Next state after lock timeout
	const byte	timeoutDS;		// Number of deciseconds to timeout

	// Output variables in this state:
	const bool	inhibitTraction:1;
	const bool	motorLocking:1;
	const bool	motorUnlocking:1;
  const bool  chargingOn:1;
	const lamps	lights;
};

#define MOTOR_DS	6

const struct State states[static_cast<size_t>(st::noChange)]  = {
//	noCpOrPp,       cpOrPp,         cpAndPp,        lockLocked,     lockUnlocked,   unlockSwitch,   timeout,      timeoutDS,		inhibitTract, motorLock,  motorUnlock,	chargingOn, lamps
	{ st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::idle,     MOTOR_DS,			true,         false,      true,         false,      lamps::all          },	// st::powerOn
	{ st::noChange,   st::cpOrPp,     st::locking,    st::noChange,   st::noChange,   st::noChange,   st::noChange, 0,						false,        false,      false,        false,      lamps::none         },  // st::idle
	{ st::noChange,   st::noChange,   st::locking,    st::noChange,   st::noChange,   st::noChange,   st::noChange, 0,						true,         false,      false,        false,      lamps::yellow       },	// st::cpOrPp
	{	st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::locked,   MOTOR_DS,			true,         true,       false,        false,      lamps::yellowFlash  },  // st::locking
	{	st::noChange,   st::noChange,   st::noChange,   st::charging,   st::unlocking,  st::unlocking,  st::noChange, 0,						true,         false,      false,        false,      lamps::yellowFlash	},  // st::locked
	{	st::noPower,    st::noPower,    st::noChange,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, 0,						true,         false,      false,        true,       lamps::green        },  // st::charging
	{	st::noChange,   st::noChange,   st::charging,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, 0,						true,         false,      false,        false,      lamps::redFlash     },  // st::noPower
	{	st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::unlocked, MOTOR_DS,			true,         false,      true,         false,      lamps::yellowFlash  },  // st::unlocking
	{	st::idle,       st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, 0,						true,         false,      false,        false,      lamps::yellow       }   // st::unlocked
};

static st state;
 byte stateTimer;

static void updateOutput();


static void updateTime()
{
	++stateTimer;
}

static SimpleTimer updateTimer(updateTime);

void initState()
{
	state = st::powerOn;
	stateTimer = 0;
	updateOutput();
	addSimpleTimer(TIMER_DS, updateTimer);
  Serial.print("****** ");
  byte idx = static_cast<byte>(state);
  Serial.print(idx);
  
  Serial.println(static_cast<int>(states[idx].timeout));
}

void updateState()
{
	auto cpIn = cpState.get();
	bool hasCp = cpIn == CP::pwm9 || cpIn == CP::pwm6;
	bool hasPp = ppState.get() != PP::invalid;
	byte stateNow = static_cast<size_t>(state);
	if (states[stateNow].noCpOrPp != st::noChange && !hasCp && !hasPp)
		state = states[stateNow].noCpOrPp;
	else if (states[stateNow].cpOrPp != st::noChange && (hasCp ^ hasPp))
		state = states[stateNow].cpOrPp;
	else if (states[stateNow].cpAndPp != st::noChange && hasCp && hasPp)
		state = states[stateNow].cpAndPp;
	else if (states[stateNow].lockLocked != st::noChange && s1State.get() == S1::locked)
		state = states[stateNow].lockLocked;
	else if (states[stateNow].lockUnlocked != st::noChange && s1State.get() == S1::unlocked)
		state = states[stateNow].lockUnlocked;
	else if (states[stateNow].unlockSwitch != st::noChange && swState.get() == SW::pressed)
		state = states[stateNow].unlockSwitch;
	else if (states[stateNow].timeout != st::noChange && stateTimer >= states[stateNow].timeoutDS)
		state = states[stateNow].timeout;
	else
		return;

	// State was changed - update output variables:
	stateTimer = 0;
	updateOutput();
}

static void updateOutput()
{
  byte stateNow = static_cast<size_t>(state);

	digitalWrite(PIN_TRACTION_DISABLE, states[stateNow].inhibitTraction);

  digitalWrite(PIN_CP_PULLDOWN, states[stateNow].chargingOn);

	digitalWrite(PIN_LOCK_OPEN, false);
	digitalWrite(PIN_LOCK_CLOSE, false);
  delay(100); // Make sure to have break-before-make to avoid shorting PSU
	digitalWrite(PIN_LOCK_OPEN, states[stateNow].motorUnlocking);
	digitalWrite(PIN_LOCK_CLOSE, states[stateNow].motorLocking);

	switch (states[stateNow].lights) {
	case lamps::none:
		digitalWrite(PIN_LED_RED, false);
		digitalWrite(PIN_LED_YELLOW, false);
		digitalWrite(PIN_LED_GREEN, false);
		break;
	case lamps::red:
	case lamps::redFlash:
		digitalWrite(PIN_LED_RED, true);
		digitalWrite(PIN_LED_YELLOW, false);
		digitalWrite(PIN_LED_GREEN, false);
		break;
	case lamps::green:
	case lamps::greenFlash:
		digitalWrite(PIN_LED_RED, false);
		digitalWrite(PIN_LED_YELLOW, false);
		digitalWrite(PIN_LED_GREEN, true);
		break;
	case lamps::yellow:
	case lamps::yellowFlash:
		digitalWrite(PIN_LED_RED, false);
		digitalWrite(PIN_LED_YELLOW, true);
		digitalWrite(PIN_LED_GREEN, false);
		break;
	case lamps::all:
	case lamps::allFlash:
		digitalWrite(PIN_LED_RED, true);
		digitalWrite(PIN_LED_YELLOW, true);
		digitalWrite(PIN_LED_GREEN, true);
		break;
	}
  
}

PGM_P getState()
{
  return stNames[static_cast<size_t>(state)];
}
