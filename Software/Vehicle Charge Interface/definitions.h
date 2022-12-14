// Pins used by the interface:

#include <Arduino.h>

#ifdef CONTROLLINO_MINI

#include <Controllino.h>

#ifndef MAIN
#define EXTERN extern
#else
#define EXTERN
#endif

// Analog input pins:
#define PIN_CP                CONTROLLINO_A0
#define PIN_CP_MUX            0
#define PIN_PP                CONTROLLINO_A1
#define PIN_PP_MUX            1
#define PIN_LOCK_SENSOR       CONTROLLINO_A2
#define PIN_LOCK_SENSOR_MUX   2

#define ADREF                 ( _BV(REFS0) )  // AD reference +5V for ATmega328P

#define N_AD_MUX_PINS         3

// Digital input pins:
#define PIN_UNLOCK_SWITCH     CONTROLLINO_A3

// Output pins:
#define PIN_LOCK_OPEN         CONTROLLINO_D0
#define PIN_LOCK_CLOSE        CONTROLLINO_D1
#define PIN_CP_PULLDOWN       CONTROLLINO_D2
#define PIN_TRACTION_DISABLE  CONTROLLINO_D4

#define PIN_LED_RED           CONTROLLINO_D3
#define PIN_LED_YELLOW        CONTROLLINO_D6
#define PIN_LED_GREEN         CONTROLLINO_D7

#else

#error "This platform is not supported."

#endif

// helpers
#define RD_LH_WRAP(REGH, REGL, VAL) { register byte reg = REGL; VAL = (static_cast<word>(REGH) << 8) | reg; }
#define RD_LH(REG, VAL) RD_LH_WRAP(REG##H, REG##L, VAL) // Read 16 bit register safely (see Atmel docs paragraph 15.3 "Accessing 16-bit Registers")

#define WR_HL_WRAP(REGH, REGL, VAL) { REGH = ((VAL)>>8)&0xff; REGL = (VAL)&0xff; }
#define WR_HL(REG, VAL) WR_HL_WRAP(REG##H, REG##L, VAL) // Write 16 bit register safely

#define MAXINPUTSTR 5

// Input variable types:
enum class CP : byte {
    pwm9 = 0      // 9V PWM (power off)
  , pwm6          // 6V PWM (power on)
  , invalid       // unconnected, no or or illegal value
};

EXTERN const char cpNames[][MAXINPUTSTR] PROGMEM
#ifdef MAIN
= {
  "off ",
  "on  ",
  "dis "
}
#endif
;

enum class PP : byte {
    max13A = 0    // 1.5 kohm resistor indicating 1.5mm2 cable for 13A max
  , max20A        // 680 ohm resistor indicating 2.5mm2 cable for 20A max
  , max32A        // 250 ohm resistor indicating 6mm2 cable for 32A max
  , invalid       // unconnected or illegal value
};

EXTERN const char ppNames[][MAXINPUTSTR] PROGMEM
#ifdef MAIN
= {
  "13A ",
  "20A ",
  "32A ",
  "dis "
}
#endif
;

enum class S1 : byte {
    unlocked = 0  // Lock state is unlocked (or locked with no plug)
  , locked        // Lock state is locked
  , invalid       // Switch state unknown
};

EXTERN const char s1Names[][MAXINPUTSTR] PROGMEM
#ifdef MAIN
= {
  "unl ",
  "lck ",
  "inv "
}
#endif
;

enum class SW : byte {
    pressed = 0   // Button is pressed (filter will act as debouncer)
  , invalid
};

EXTERN const char swNames[][MAXINPUTSTR] PROGMEM
#ifdef MAIN
= {
  "* ",
  "- "
}
#endif
;

// processed input variables:

EXTERN int16_t  pwmValue;         // Measured duty cycle in thousands
EXTERN byte     pwmCurrent;       // Max current calculated from duty cycle

// input filter:
template<typename Enum, byte depth = 10, byte invalid = static_cast<byte>(Enum::invalid)>
class Filter
{
public:
  Filter(PGM_P const names) : names(names)
  {
    memset(weights, 0, sizeof(weights));
    weights[invalid] = depth;
  }
  void set(Enum value)
  {
    bool set=false;
    for (byte w=0; w<=invalid; ++w) {
      if (w==static_cast<byte>(value) || (w == invalid && !set)) {
        if (weights[w]<depth) ++weights[w];
        if (weights[w] == depth) {
          current=static_cast<Enum>(w);
          set = true;
          if (w != invalid) weights[invalid] = 0;
        }
      }
      else {
        if (weights[w]>2)
          weights[w] -= 2;
        else
          weights[w] = 0;
      }
    }
  }
  Enum get() const
  {
    return current;
  }
  PGM_P getValue()
  {
    return names + static_cast<size_t>(current)*MAXINPUTSTR;
  }
private:
  byte weights[invalid+1];
  Enum current = Enum::invalid;
  PGM_P names;
};

// Typed PROGMEM byte access:
template<typename T>
T rdPgm(const T& val) {
  return static_cast<T>(pgm_read_byte(&val));
}


#ifdef MAIN
Filter<CP> cpState(cpNames[0]);     // State of the CP signal
Filter<PP> ppState(ppNames[0]);     // State of the PP signal
Filter<S1> s1State(s1Names[0]);     // State of the S1 signal
Filter<SW> swState(swNames[0]);     // State of the switch
#else
extern Filter<CP> cpState;          // State of the CP signal
extern Filter<PP> ppState;          // State of the PP signal
extern Filter<S1> s1State;          // State of the S1 signal
extern Filter<SW> swState;          // State of the switch
#endif
// prototypes
void writePGM(PGM_P p);

// ad.cpp:
void initAD();
void startAD(bool highCP);

// state.cpp:
void initState();
void updateState();
PGM_P getState();

// timer.cpp:

#define TIMER_CNT     5
#define TIMER_TOP_16  (F_CPU/8/1000)      // Top value for 16-bit timers for 1kHz loop with factor 8 prescaling
#define TIMER_TOP_8   (F_CPU/64/1000)     // Top value for 8-bit timers for 1kHz loop with factor 64 prescaling
#define TIMER_TOP TIMER_TOP_16            // Value used for PWM calculation always relates to 16-bit timer to get good correct resolution
#define PWM_DIVISOR   (TIMER_TOP_16/100)  // Divisor for calculating pwm% from counter value

// Timer type identifiers
#define TIMER_MS    0
#define TIMER_CS    1
#define TIMER_DS    2
#define TIMER_CNT   3

typedef void (*TimerFunc)();

struct SimpleTimer {
  inline SimpleTimer(TimerFunc call) : call(call) {};
  TimerFunc call;
  struct SimpleTimer* next;
};

struct ComplexTimer {
  inline ComplexTimer(TimerFunc call, uint16_t cnt, bool recurring) : call(call), count(cnt), recurring(recurring), rest(cnt) {};
  TimerFunc call;
  uint16_t  rest;
  uint16_t  count;
  bool      recurring;
  struct ComplexTimer* next;
};

void initTimers();
void addSimpleTimer(byte unit, SimpleTimer& timer);
void addComplexTimer(byte unit, ComplexTimer& timer);
void setComplexTimer(ComplexTimer& timer, uint16_t count, bool recurring);

// Helper for oring bits:
constexpr uint16_t orBits() { return 0; }

template<typename... T>
constexpr uint16_t orBits(uint16_t n, T... rest)
{
  return (1<<n) | orBits(rest...);
}

