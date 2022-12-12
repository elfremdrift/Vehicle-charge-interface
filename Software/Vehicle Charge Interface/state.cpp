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
	, nStates
	, noChange
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
	const st		noCpNorPp;		// Next state if neither CP nor PP present
	const st		cpOrPp;				// Next state if either CP or PP present
	const st		cpAndPp;			// Next state if both CP and PP present
	const st		lockLocked;		// Next state if lock indicates locked
	const st		lockUnlocked;	// Next state if lock indicates unlocked
	const st		unlockSwitch;	// Next state if unlock switch is pressed
	const st		timeout;			// Next state after lock timeout
	const byte	timeoutCS;		// Number of centiseconds to timeout

	// Output variables in this state:
	const bool	inhibitTraction:1;
	const bool	motorLocking:1;
	const bool	motorUnlocking:1;
	const lamps	lights;
};

#define MOTOR_DS	6

const struct State states[static_cast<size_t>(st::noChange)] PROGMEM = {
//	noCpOrPp,       cpOrPp,         cpAndPp,        lockLocked,     lockUnlocked,   unlockSwitch,   timeout,      timeoutDS,		inhibitTract, motorLock,  motorUnlock,	lamps
	{ st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::idle,     MOTOR_DS,			true,         false,      true,         lamps::all          },	// st::powerOn
	{ st::noChange,   st::cpOrPp,     st::locking,    st::noChange,   st::noChange,   st::noChange,   st::noChange, 0,						false,        false,      false,        lamps::none         },  // st::idle
	{ st::noChange,   st::noChange,   st::locking,    st::noChange,   st::noChange,   st::noChange,   st::noChange, 0,						true,         false,      false,        lamps::yellow       },	// st::cpOrPp
	{	st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::locked,   MOTOR_DS,			true,         true,       false,        lamps::yellowFlash  },  // st::locking
	{	st::noChange,   st::noChange,   st::noChange,   st::charging,   st::unlocking,  st::unlocking,  st::noChange, 0,						true,         false,      false,        lamps::yellowFlash	},  // st::locked
	{	st::noPower,    st::noPower,    st::noChange,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, 0,						true,         false,      false,        lamps::green        },  // st::charging
	{	st::noChange,   st::noChange,   st::charging,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, 0,						true,         false,      false,        lamps::redFlash     },  // st::noPower
	{	st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::unlocked, MOTOR_DS,			true,         false,      true,         lamps::yellowFlash  },  // st::unlocking
	{	st::idle,       st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, 0,						true,         false,      false,        lamps::yellow       }   // st::unlocked
};

static st state;
static byte stateTimer;

static void updateTime()
{
	++stateTimer;
}

static void SimpleTimer updateTimer(updateTime);

void initState()
{
	state = st::powerOn;
	stateTimer = 0;
	updateOutput();
	addSimpleTimer(TIMER_DS, &updateTimer);
}

void updateState()
{
	auto cpIn = cp.get();
	bool hasCp = cpIn == CP::pwm9 || cpIn == CP::pwm6;
	bool hasPp = pp.get() != PP::invalid;
	byte stateNow = static_cast<size_t>(state);
	if (states[stateNow].noCpOrPp != st::noChange && !hasCp && !hasPp)
		state = states[stateNow].noCpOrPp;
	else if (states[stateNow].cpOrPp != st::noChange && (hasCp || hasPp))
		state = states[stateNow].cpOrPp;
	else if (states[stateNow].cpAndPp != st::noChange && hasCp && hasPp)
		state = states[stateNow].cpAndPp;
	else if (states[stateNow].lockLocked != st::noChange && S1.get() == S1::locked)
		state = states[stateNow].lockLocked;
	else if (states[stateNow].lockUnlocked != st::noChange && S1.get() == S1::unlocked)
		state = states[stateNow].lockUnlocked;
	else if (states[stateNow].unlockSwitch != st::noChange && SW.get() == SW::pressed)
		state = states[stateNow].unlockSwitch;
	else if (states[stateNow].timeout != st::noChange && stateTimer >= states[stateNow].timeoutDS)
		state = states[stateNow].timeout;
	else
		return;

	// State was changed - update output variables:
	stateTimer = 0;
	updateOutput();
}

void updateOutput()
{
	byte stateNow = static_cast<size_t>(state);
	digitalWrite(PIN_TRACTION_DISABLE, states[stateNow].inhibitTraction);
	// TODO: Make sure to have break-before-make to avoid shorting PSU
	digitalWrite(PIN_LOCK_OPEN, states[stateNow].motorUnlock);
	digitalWrite(PIN_LOCK_CLOSE, states[stateNow].motorLock);

	switch (states[stateNow].lamps) {
	case none:
		digitalWrite(PIN_LAMP_RED, false);
		digitalWrite(PIN_LAMP_YELLOW, false);
		digitalWrite(PIN_LAMP_GREEN, false);
		break;
	case red:
	case redFlash:
		digitalWrite(PIN_LAMP_RED, true);
		digitalWrite(PIN_LAMP_YELLOW, false);
		digitalWrite(PIN_LAMP_GREEN, false);
		break;
	case green:
	case greenFlash:
		digitalWrite(PIN_LAMP_RED, false);
		digitalWrite(PIN_LAMP_YELLOW, false);
		digitalWrite(PIN_LAMP_GREEN, true);
		break;
	case yellow:
	case yellowFlash:
		digitalWrite(PIN_LAMP_RED, false);
		digitalWrite(PIN_LAMP_YELLOW, true);
		digitalWrite(PIN_LAMP_GREEN, false);
		break;
	case all:
	case allFlash:
		digitalWrite(PIN_LAMP_RED, true);
		digitalWrite(PIN_LAMP_YELLOW, true);
		digitalWrite(PIN_LAMP_GREEN, true);
		break;
	}
}
