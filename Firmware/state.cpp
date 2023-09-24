// Charger state machine

#include "definitions.h"

enum class st : byte {
    powerOn = 0       // [ALL] Initial state when powered on - do initial unlock
  , idle              // [OFF] Idle state - waiting for plug
  , cpOrPp            // [YELLOW] Either CP or PP found - waiting for both
  , locking           // [YELLOW] Start locking
  , locked            // [YELLOW] Motor ran for MOTOR_DS deciseconds - check if locked correctly
  , charging          // [GREEN] Locked correctly - power turned on
  , noPower           // [RED] Power was lost after charging started
  , unlocking         // [YELLOW] Start unlocking
  , unlockwt          // [YELLOW] Motor ran MOTOR_DS - stop motor, wait here until switch released
  , unlocked          // [YELLOW] Switch released, waiting for CP and PP to go away, then back to idle
  , unlckerr          // [RED] Locking failed - unlock again
  , unlckwterr        // [RED] Lock should be unlocked - wait here if switch is pressed
  , unlckderr         // [RED] Switch released, wait for CP and PP to go away, then back to idle
  , noChange  // Must be last
};

const char stNames[][static_cast<byte>(st::noChange)] PROGMEM = {
    "powerOn   "
  , "idle      "
  , "cpOrPp    "
  , "locking   "
  , "locked    "
  , "charging  "
  , "noPower   "
  , "unlocking "
  , "unlockwt  "
  , "unlocked  "
  , "unlkerr   "
  , "unlkwterr "
  , "unlkderr  "
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
  { st::idle,     st::noChange, st::locking,  st::noChange, st::noChange,   st::noChange,   st::noChange,   0,          true,         false,      false,        false,      lamps::yellow       },  // st::cpOrPp
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::locked,     MOTOR_DS,   true,         true,       false,        false,      lamps::yellowFlash  },  // st::locking
  { st::noChange, st::noChange, st::noChange, st::charging, st::unlckerr,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::yellowFlash  },  // st::locked
  { st::noPower,  st::noPower,  st::noChange, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        true,       lamps::green        },  // st::charging
  { st::noChange, st::noChange, st::charging, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::redFlash     },  // st::noPower
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::unlockwt,   MOTOR_DS,   true,         false,      true,         false,      lamps::yellowFlash  },  // st::unlocking
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::unlockwt,   st::unlocked,   2,          true,         false,      false,        false,      lamps::yellow       },  // st::unlockwt
  { st::idle,     st::noChange, st::noChange, st::noChange, st::noChange,   st::unlocking,  st::noChange,   0,          true,         false,      false,        false,      lamps::yellow       },  // st::unlocked
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::noChange,   st::unlckwterr, MOTOR_DS,   true,         false,      true,         false,      lamps::redFlash     },  // st::unlocking
  { st::noChange, st::noChange, st::noChange, st::noChange, st::noChange,   st::unlckwterr, st::unlckderr,  2,          true,         false,      false,        false,      lamps::redFlash     },  // st::unlocking
  { st::idle,     st::noChange, st::noChange, st::noChange, st::noChange,   st::unlckerr,   st::noChange,   0,          true,         false,      false,        false,      lamps::redFlash     }   // st::unlocked
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

void dumpState()
{
  static byte cnt = 0;
  static st lastST = static_cast<st>(255);
  static CP lastCP = static_cast<CP>(255);
  static PP lastPP = static_cast<PP>(255);
  static S1 lastS1 = static_cast<S1>(255);
  static SW lastSW = static_cast<SW>(255);
  if (state == lastST && cpState.get() == lastCP && ppState.get() == lastPP && s1State.get() == lastS1 && swState.get() == lastSW) return;

  if ((cnt++ & 0x0f) == 0) {
      writePGM(PSTR("State     CP  PP  S1  SW R Y G\n"));
  }

  writePGM(getState());
  writePGM(cpState.getValue());
  writePGM(ppState.getValue());
  writePGM(s1State.getValue());
  writePGM(swState.getValue());

  lastST = state;
  lastCP = cpState.get();
  lastPP = ppState.get();
  lastS1 = s1State.get();
  lastSW = swState.get();

  const State& stateNow = states[static_cast<byte>(state)];
  
  switch (rdPgm(stateNow.lights)) {
  case lamps::none:
    writePGM(PSTR("- - - "));
    break;
  case lamps::red:
    writePGM(PSTR("* - - "));
    break;
  case lamps::redFlash:
    writePGM(PSTR("! - - "));
    break;
  case lamps::green:
    writePGM(PSTR("- - * "));
    break;
  case lamps::greenFlash:
    writePGM(PSTR("- - ! "));
    break;
  case lamps::yellow:
    writePGM(PSTR("- * - "));
    break;
  case lamps::yellowFlash:
    writePGM(PSTR("- ! - "));
    break;
  case lamps::all:
    writePGM(PSTR("* * * "));
    break;
  case lamps::allFlash:
    writePGM(PSTR("! ! ! "));
    break;
  }
  
  Serial.println();
  //Serial.println(stateTimer);
  //Serial.print("State -> "); Serial.print(static_cast<int>(state));
  //Serial.print(" PP: "); Serial.print(adConversions[0]);
  //Serial.print(" pp: "); Serial.println(adConversions[1]);
  Serial.flush();
}

void updateState()
{
  auto cpIn = cpState.get();
  bool hasCp = cpIn == CP::pwm9 || cpIn == CP::pwm6;
  bool hasPp = ppState.get() != PP::invalid;
  const State& stateNow = states[static_cast<byte>(state)];
  st nextSt;

  dumpState();

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

  dumpState();

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
