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
  ICR##N = TIMER_TOP_16*10/9;  // In this application, we freecycle a bit slower than 1KHz (900Hz) and manually reset timer when CP goes high

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
#define TIMER_REG_AB_WRAP(reg,timer,ab) reg##timer##ab
#define TIMER_REG_AB(reg,timer,ab) TIMER_REG_AB_WRAP(reg,timer,ab)

#define INITTIMER_WRAP3(X,B,I) INITTIMER_##B##_WRAP(X,I)
#define INITTIMER_WRAP2(X,B,I) INITTIMER_WRAP3(X,B,I)
#define INITTIMER_WRAP(X, I) INITTIMER_WRAP2(X, TIMER_BITS_##X, I)
#define INITTIMER(X, I) INITTIMER_WRAP(X, I)

#define REG2_WRAP(R, N) R##N
#define REG2(R,N) REG2_WRAP(R,N)
#define REG3_WRAP(R, N, C) R##N##C
#define REG3(R,N,C) REG3_WRAP(R,N,C)

#define  TIMER_05PCT_16  (5*TIMER_TOP_16/100)    // Value at 5% of ramp for measuring high CP
#define  TIMER_90PCT_16  (90*TIMER_TOP_16/100)   // Value at 90 % of ramp for measuruning low CP

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
  WR_HL(TCNT1, 0);  // Init timer 1 value to 0
  REG3(OCR,TIMER_A,A) = TIMER_05PCT_16;  // Give interrupt at 5% into PWM cycle to measure high CP
  REG3(OCR,TIMER_A,B) = TIMER_90PCT_16;  // Give interrupt at 90% into PWM cycle to measure low CP

  GTCCR = 0;  // Release timers

  sei();  // Re-enable interrupts

  // Let timer run freely for a short period of time as OCR flags in this mode are set at timer TOP value
  delay(100);

  // Then allow CP to interrupt us:
  PCMSK0 = 0;
  PCMSK1 = orBits(PCINT8); // Allow interrupt on CP changes
  PCMSK2 = 0;
  PCICR = orBits(PCIE1);

  // Piggyback on timer0:
  OCR0B = 10;   // Call our interrupt when timer reaches arbitrary value of 10
  TIFR0 = 0;  // Enable interrupt on OCR0B value match (every 125/128 ms ~ 1ms)
  TIMSK0 |= _BV(OCIE0B);
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

ISR(TIMER0_COMPB_vect)    // 5% into duty-cycle
{
  MsTimer();    // We piggyback on the standard arduino Timer0 to get an appx. 1ms interrupt for our timers
}

ISR(TIMER1_COMPA_vect)    // 5% into duty-cycle
{
  startAD(true);          // Measure high CP value
}

ISR(TIMER1_COMPB_vect)    // 90% into dutycycle
{
  startAD(false);         // Measure low CP value
}

static void timerSync()
{
#ifdef PSRSYNC
  GTCCR = orBits(TSM, PSRSYNC); // Stop timers for sync
#else
  GTCCR = orBits(TSM);
#endif

  WR_HL(TCNT1, 0);  // Set timer value to 0

  GTCCR = 0;  // Release timers
}

static void readPWM()
{
  RD_LH(TCNT1, pwmValue);
}

// CP sensing interrupt
ISR(PCINT1_vect)
{
  if (PINC & _BV(PC0)) {
    // Leading edge of CP pulse: Syncronize timer with CP signal
    timerSync();
  }
  else {
    // Falling edge of CP pulse: Register PWM value. Clock is set to 1MHz so PWM in % is value / 10
    readPWM();
  }
}

