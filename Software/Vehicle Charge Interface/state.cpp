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
	const st	noCpNorPp;		// Next state if neither CP nor PP present
	const st  cpOrPp;				// Next state if either CP or PP present
	const st	cpAndPp;			// Next state if both CP and PP present
	const st	lockLocked;			// Next state if lock indicates locked
	const st lockUnlocked;		// Next state if lock indicates unlocked
	const st	unlockSwitch;		// Next state if unlock switch is pressed
	const st	timeout;			// Next state after lock timeout

	// Output variables in this state:
	const bool	inhibitTraction:1;
	const bool	motorLocking:1;
	const bool	motorUnlocking:1;
	const lamps	lights;
};

st state = st::powerOn;

const struct State states[static_cast<size_t>(st::noChange)] PROGMEM = {
//	noCpOrPp,       cpOrPp,         cpAndPp,        lockLocked,     lockUnlocked,   unlockSwitch,   timeout,      inhibitTract, motorLock,  motorUnlock,	lamps
	{ st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::idle,     true,         false,      true,         lamps::all          },	// st::powerOn
	{ st::noChange,   st::cpOrPp,     st::locking,    st::noChange,   st::noChange,   st::noChange,   st::noChange, false,        false,      false,        lamps::none         },  // st::idle
	{ st::noChange,   st::noChange,   st::locking,    st::noChange,   st::noChange,   st::noChange,   st::noChange, true,         false,      false,        lamps::yellow       },	// st::cpOrPp
	{	st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::locked,   true,         true,       false,        lamps::yellowFlash  },  // st::locking
	{	st::noChange,   st::noChange,   st::noChange,   st::charging,   st::unlocking,  st::unlocking,  st::noChange, true,         false,      false,        lamps::yellowFlash	},  // st::locked
	{	st::noPower,    st::noPower,    st::noChange,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, true,         false,      false,        lamps::green        },  // st::charging
	{	st::noChange,   st::noChange,   st::charging,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, true,         false,      false,        lamps::redFlash     },  // st::noPower
	{	st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::unlocked, true,         false,      true,         lamps::yellowFlash  },  // st::unlocking
	{	st::idle,       st::noChange,   st::noChange,   st::noChange,   st::noChange,   st::unlocking,  st::noChange, true,         false,      false,        lamps::yellow       }   // st::unlocked
};

