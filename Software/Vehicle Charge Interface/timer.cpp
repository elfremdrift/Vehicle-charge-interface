#include "definitions.h"

#define N_PORTS 1

// Some lowlevel timer helpers:

#define UNWRAP(X) X

#define INITTIMER_8_WRAP(N,I) \
  TCCR##N##A = orBits(WGM##N##1, WGM##N##0); \
  TCCR##N##B = orBits(WGM##N##2, CS##N##2); \
  TIMSK##N = I; \
  TIFR##N = orBits();

#define INITTIMER_16_WRAP(N, I) \
  TCCR##N##A = orBits(WGM##N##1); \
  TCCR##N##B = orBits(WGM##N##3, WGM##N##2, CS##N##1 /*, CS##N##0,*/); \
  TCCR##N##C = orBits(); \
  TIMSK##N = I; \
  TIFR##N = orBits(); \
  ICR##N = TIMER_TOP;

#define TIMER_TOP_16  (F_CPU/8/1000)  // Top value for 16-bit timers for 1kHz loop with factor 8 prescaling
#define TIMER_TOP_8   (F_CPU/64/1000) // Top value for 8-bit timers for 1kHz loop with factor 64 prescaling
#define TIMER_TOP TIMER_TOP_16        // Value used for PWM calculation always relates to 16-bit timer to get good correct resolution

#define TIMER_BITS_0  8
#define TIMER_BITS_1  16
#define TIMER_BITS_2  8
#define TIMER_BITS_3  16
#define TIMER_BITS_4  16
#define TIMER_BITS_6  16

#define TIMER_BITS_WRAP(timer) TIMER_BITS_##timer
#define TIMER_BITS(timer) TIMER_BITS_WRAP(timer)

#define TIMER_REG_WRAP(reg,timer) reg##timer
#define TIMER_REG(reg,timer) TIMER_REG_WRAP(reg,timer)
#define TIMER_REGAB_WRAP(reg,timer,ab) reg##timer##ab
#define TIMER_REGAB(reg,timer,ab) TIMER_REG_WRAP(reg,timer,ab)

#define INITTIMER_WRAP3(X,B,I) INITTIMER_##B##_WRAP(X,I)
#define INITTIMER_WRAP2(X,B,I) INITTIMER_WRAP3(X,B,I)
#define INITTIMER_WRAP(X, I) INITTIMER_WRAP2(X, TIMER_BITS_##X, I)
#define INITTIMER(X, I) INITTIMER_WRAP(X, I)

#define REG2_WRAP(R, N) R##N
#define REG2(R,N) REG2_WRAP(R,N)
#define REG3_WRAP(R, N, C) R##N##C
#define REG3(R,N,C) REG3_WRAP(R,N,C)

#define	TIMER_05PCT_16  (5*TIMER_TOP_16/100)    // Value at 5% of ramp for measuring high CP
#define	TIMER_90PCT_16  (90*TIMER_TOP_16/100)   // Value at 90 % of ramp for measuruning low CP

static int lastOutput[N_PORTS];

#define TIMER_A  1

static const byte timerMap[N_PORTS] PROGMEM = { TIMER_A };

volatile uint16_t msCount = 0;
volatile uint16_t csCount = 0;
volatile uint16_t dsCount = 0;
volatile uint16_t sCount = 0;

static byte ms10 = 0;
static byte cs10 = 0;
static byte ds10 = 0;

static SimpleTimer* simpleTimers[TIMER_CNT] = {nullptr, nullptr, nullptr};
static ComplexTimer* complexTimers[TIMER_CNT] = {nullptr, nullptr, nullptr};

void initTimers()
{
  memset(lastOutput, 0xff, sizeof(lastOutput));
  
  cli();  // Stop interrupts

#ifdef PSRSYNC
  GTCCR = orBits(TSM, PSRSYNC); // Stop timers for sync 
#else
  GTCCR = orBits(TSM); // Stop timers for sync 
#endif

  // Set up timers to use fast PWM, top register
  INITTIMER(TIMER_A, orBits(TIMER_REG_AB(OCIE, TIMER_A, A), TIMER_REG_AB(OCIE, TIMER_A, B)));
  REG2(TCNT,TIMER_A) = 0;  // Init timer A value to 0
  REG3(OCR,TIMER_A,A) = TIMER_05PCT_16;	// Give interrupt at 5% into PWM cycle to measure high CP
  REG3(OCR,TIMER_A,B) = TIMER_05PCT_16;	// Give interrupt at 90% into PWM cycle to measure low CP

  GTCCR = 0;  // Release timers

  sei();  // Re-enable interrupts
}

void addSimpleTimer(byte unit, SimpleTimer& timer)
{
  timer.next = simpleTimers[unit];
  simpleTimers[unit] = &timer;
}

void addComplexTimer(byte unit, ComplexTimer& timer)
{
  timer.next = complexTimers[unit];
  complexTimers[unit] = &timer;
}

void setComplexTimer(ComplexTimer& timer, uint16_t count, bool recurring)
{
  cli();
  timer.count = timer.rest = count;
  timer.recurring = recurring;
  sei();
}

inline void runTimerCalls(byte timerUnit)
{
  SimpleTimer* t = simpleTimers[timerUnit];
  while (t) {
    (*t->call)();
    t = t->next;
  }

  ComplexTimer* c = complexTimers[timerUnit];
  while (c) {
    if (c->count != 0 && c->rest != 0) {
      c->rest--;
      if (c->rest == 0) {
        (*c->call)();
        if (c->recurring) {
          c->rest = c->count;
        }
      }
    }
    c++;
  }
}
extern uint16_t conversions;
void MsTimer()
{
  runTimerCalls(TIMER_MS);
  if (ms10 != 9) {
    ms10++;
  } else {
    ms10 = 0;
    runTimerCalls(TIMER_CS);
    if (cs10 != 9) {
      cs10++;
    } else {
      cs10 = 0;
      runTimerCalls(TIMER_DS);
      if (ds10 != 9) {
        ds10++;
      } else {
        ds10 = 0;
        // runTimerCalls(TIMER_S);
        sCount++;
      }
      dsCount++;
    }
    csCount++;
  }
  msCount++;
}

#define ISR_ROUTINE_WRAP(N,I,F,AB,M) \
ISR(TIMER##N##_##I##AB##_vect) \
{ \
  TIFR##N = TIFR##N & ~F##N##AB; \
  M \
}
#define ISR_ROUTINE(N,I,F,AB,M) ISR_ROUTINE_WRAP(N,I,F,AB,M)

ISR_ROUTINE(TIMER_A, OVF, TOV, , MsTimer(););
ISR_ROUTINE(TIMER_A, COMP, OCF, A, startAD(true););
ISR_ROUTINE(TIMER_A, COMP, OCF, B, startAD(false););

