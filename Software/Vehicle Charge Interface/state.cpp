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
  , unlckerr
  , unlckderr
  , noChange  // Must be last
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
  , "unlckerr  "
  , "unlckderr "
  , "noChange  "
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
  const st    noCpOrPp;     // Next state if neither CP nor PP present
  const st    cpOrPp;       // Next state if either CP or PP present
  const st    cpAndPp;      // Next state if both CP and PP present
  const st    lockLocked;   // Next state if lock indicates locked
  const st    lockUnlocked; // Next state if lock indicates unlocked
  const st    unlockSwitch; // Next state if unlock switch is pressed
  const st    timeout;      // Next state after lock timeout
  const byte  timeoutDS;    // Number of deciseconds to timeout

  // Output variables in this state:
  const byte  inhibitTraction;
  const byte  motorLocking;
  const byte  motorUnlocking;
  const byte  chargingOn;
  const lamps lights;
};

#define MOTOR_DS  6

const struct State states[static_cast<size_t>(st::noChange)] PROGMEM = {
//  noCpOrPp,     cpOrPp,       cpAndPp,      lockLocked,   lockUnlocked,   unlockSwitch,   timeout,        timeoutDS,  inhibitTract, motorLock,  motorUnlock,  chargingOn, lamps
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::idle,       MOTOR_DS,   false,        false,      true,         false,      lamps::all          },  // st::powerOn
  { st::noChange, st::cpOrPp,   st::locking,  st::noChange, st::noChange,   st::noChange,   st::noChange,   0,          false,        false,      false,        false,      lamps::none         },  // st::idle
  { st::noChange, st::noChange, st::locking,  st::noChange, st::noChange,   st::noChange,   st::noChange,   0,          true,         false,      false,        false,      lamps::yellow       },  // st::cpOrPp
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::locked,     MOTOR_DS,   true,         true,       false,        false,      lamps::yellowFlash  },  // st::locking
  { st::noChange, st::noChange, st::noChange, st::charging, st::unlckerr,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::yellowFlash  },  // st::locked
  { st::noPower,  st::noPower,  st::noChange, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        true,       lamps::green        },  // st::charging
  { st::noChange, st::noChange, st::charging, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::redFlash     },  // st::noPower
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::unlocked,   MOTOR_DS,   true,         false,      true,         false,      lamps::yellowFlash  },  // st::unlocking
  { st::idle,     st::noChange, st::noChange, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::yellow       },  // st::unlocked
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::unlckderr,  MOTOR_DS,   true,         false,      true,         false,      lamps::redFlash     },  // st::unlocking
  { st::idle,     st::noChange, st::noChange, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::redFlash     }   // st::unlocked
};

static st state;
byte stateTimer;

static void updateOutput();

static void updateTime()
{
  if (stateTimer < 255)
    ++stateTimer;
}

static void flash()
{
  switch (rdPgm(states[static_cast<byte>(state)].lights)) {
  case lamps::redFlash:
    digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
    break;
  case lamps::greenFlash:
    digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
    break;
  case lamps::yellowFlash:
    digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
    break;
  case lamps::allFlash:
    digitalWrite(PIN_LED_RED, !digitalRead(PIN_LED_RED));
    digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
    digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
    break;
  }
}

static SimpleTimer updateTimer(updateTime);
static ComplexTimer flashTimer(flash, 3, true);

void initState()
{
  state = st::powerOn;
  stateTimer = 0;
  updateOutput();
  addSimpleTimer(TIMER_DS, updateTimer);
  addComplexTimer(TIMER_DS, flashTimer);
}

void updateState()
{
  auto cpIn = cpState.get();
  bool hasCp = cpIn == CP::pwm9 || cpIn == CP::pwm6;
  bool hasPp = ppState.get() != PP::invalid;
  const State& stateNow = states[static_cast<size_t>(state)];
  st nextSt;
  if ((nextSt=rdPgm(stateNow.unlockSwitch)) != st::noChange && swState.get() == SW::pressed)
    state = nextSt;
  else if ((nextSt=rdPgm(stateNow.noCpOrPp)) != st::noChange && !hasCp && !hasPp)
    state = nextSt;
  else if ((nextSt=rdPgm(stateNow.cpOrPp)) != st::noChange && (hasCp ^ hasPp))
    state = nextSt;
  else if ((nextSt=rdPgm(stateNow.cpAndPp)) != st::noChange && hasCp && hasPp)
    state = nextSt;
  else if ((nextSt=rdPgm(stateNow.lockLocked)) != st::noChange && s1State.get() == S1::locked)
    state = nextSt;
  else if ((nextSt=rdPgm(stateNow.lockUnlocked)) != st::noChange && s1State.get() == S1::unlocked)
    state = nextSt;
  else if ((nextSt=rdPgm(stateNow.timeout)) != st::noChange && stateTimer >= rdPgm(stateNow.timeoutDS))
    state = nextSt;
  else
    return;

  // State was changed - update output variables:
  stateTimer = 0;
  updateOutput();
}

static void updateOutput()
{
  const State& stateNow = states[static_cast<size_t>(state)];

  digitalWrite(PIN_TRACTION_DISABLE, rdPgm(stateNow.inhibitTraction));

  digitalWrite(PIN_CP_PULLDOWN, rdPgm(stateNow.chargingOn));

  byte newState = (rdPgm(stateNow.motorLocking) ? 1 : 0) + (rdPgm(stateNow.motorUnlocking) ? 2 : 0);
  byte oldState = (digitalRead(PIN_LOCK_CLOSE) ? 1 : 0) + (digitalRead(PIN_LOCK_OPEN) ? 2 : 0);

  if (oldState != newState) {
    digitalWrite(PIN_LOCK_OPEN, false);
    digitalWrite(PIN_LOCK_CLOSE, false);
    if (newState != 0) {
      if (oldState != 0) delay(100); // Ensure break-before-make
      digitalWrite(PIN_LOCK_CLOSE, newState & 1);
      digitalWrite(PIN_LOCK_OPEN, newState & 2);
    }
  }

  switch (rdPgm(stateNow.lights)) {
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
