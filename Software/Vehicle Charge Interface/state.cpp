// Charger state machine

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
}

struct State {
	// Next state based on read values;
	state_t	noCpNorPp;		// Next state if neither CP nor PP present
	state_t	cpOrPp;				// Next state if either CP or PP present
	state_t	cpAndPp;			// Next state if both CP and PP present
	state_t	lockLocked;			// Next state if lock indicates locked
	state_t lockUnlocked;		// Next state if lock indicates unlocked
	state_t	unlockSwitch;		// Next state if unlock switch is pressed
	state_t	timeout;			// Next state after lock timeout

	// Output variables in this state:
	bool	inhibitTraction:1;
	bool	motorLocking:1;
	bool	motorUnlocking:1;
	byte	lights;
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

state_t state = state_t:powerOn;

struct State[static_cast<size_t>(state_t::noChange)] states PROGMEM = {
//		noCpOrPp,		cpOrPp,			cpAndPp,		lockLocked,		lockUnlocked,	unlockSwitch,	timeout
//			inhibitTract,	motorLock,		motorUnlock,	lamps
	{	st::noChange,	st::noChange,	st::noChange,	st::noChange,	st::noChange,	st:noChange,	st::idle,		// st::powerOn
			true,			false			true			lamps::all		},
	{	st::noChange,	st::cpOrPp,		st::locking,	st::noChange,	st::noChange,	st:noChange,	st::noChange,	// st::idle
			false,			false,			false,			lamps::none		},
	{	st::noChange,	st::noChange,	st::locking,	st::noChange,	st::noChange,	st:noChange,	st::noChange,	// st::cpOrPp
			true,			false,			false,			lamps::yellow	},
	{	st::noChange,	st::noChange,	st::noChange,	st::noChange,	st::noChange,	st:noChange,	st::locked,		// st::locking
			true,			true,			false,			lamps::yellowFlash	},
	{	st::noChange,	st::noChange,	st::noChange,	st::charging,	st::unlock,		st:unlock,		st::noChange,	// st::locked
			true,			false,			false,			lamps::yellowFlash	},
	{	st::noPower,	st::noPower,	st::noChange,	st::noChange,	st::noChange,	st:unlock,		st::noChange,	// st::charging
			true,			false,			false,			lamps::green		},
	{	st::noChange,	st::noChange,	st::charge,		st::noChange,	st::noChange,	st:unlock,		st::noChange,	// st::noPower
			true,			false,			false,			lamps::redFlash		},
	{	st::noChange,	st::noChange,	st::noChange,	st::noChange,	st::noChange,	st:noChange,	st::unlocked,	// st::unlocking
			true,			false,			true,			lamps::yellowFlash	},
	{	st::idle,		st::noChange,	st::noChange,	st::noChange,	st::noChange,	st:unlock,		st::noChange,	// st::unlocked
			true,			false,			false,			lamps::yellow		}
};

