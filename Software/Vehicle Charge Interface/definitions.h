// Pins used by the interface:

#include <Arduino.h>

#ifdef CONTROLLINO_MINI

#include <Controllino.h>

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


// prototypes

// ad.cpp:
void initAD();    
void startAD(bool highCP);

// timer.cpp:

#define TIMER_CNT	5
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
  inline ComplexTimer(TimerFunc call, uint16_t count, bool recurring) : call(call), count(count), recurring(recurring), rest(0) {};
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

extern uint16_t pwmValue;

// Helper for oring bits:
constexpr uint16_t orBits() { return 0; }

template<typename... T>
constexpr uint16_t orBits(uint16_t n, T... rest)
{
  return (1<<n) | orBits(rest...);
} 

