// A/D conversions for pin measurements are handled here

#include "definitions.h"

uint16_t  adConversions[N_AD_MUX_PINS + 1] = { 0, 0, 0, 0 };

static byte nextPort = 0;
static bool highCP = false;

uint16_t conversions = 0;

static SimpleTimer startADTimer(startAD);

void initAD()
{
  // We set up the AD converter manually to have it run in the background
  ADCSRA = orBits(ADEN, ADIE, ADPS2);
  ADMUX = ADREF | nextPort;
  DIDR0 = orBits(PIN_CP_MUX, PIN_PP_MUX, PIN_LOCK_SENSOR_MUX);

  addSimpleTimer(TIMER_CS, startADTimer);
}

void startAD(bool highCPnext)
{
  nextPort = 0;
  highCP = highCPnext;
  ADMUX = ADREF | nextPort;
  ADCSRA |= _BV(ADSC); // Start conversion
  ++conversions;
}

ISR(ADC_vect)
{
  // Latest AD conversion done.
  ADCSRA &= ~_BV(ADIF); // Reset interrupt flag
  adConversions[nextPort+(highCP?0:1)] = ADCW+1;

  ++nextPort;
  highCP = false;
  if (nextPort != N_AD_MUX_PINS) {
    ADMUX = ADREF | nextPort;
    ADCSRA |= _BV(ADSC); // Start conversion
  }
}
