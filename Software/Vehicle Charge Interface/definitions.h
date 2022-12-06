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

// prototypes

// ad.cpp:
void initAD();    
void startAD();

// Helper for oring bits:
constexpr uint16_t orBits(uint16_t n) { return 1<<n; }

template<typename... T>
constexpr uint16_t orBits(uint16_t n, T... rest)
{
  return (1<<n) | orBits(rest...);
} 

