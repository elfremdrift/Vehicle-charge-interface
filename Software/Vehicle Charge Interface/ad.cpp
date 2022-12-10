// A/D conversions for pin measurements are handled here

#include "definitions.h"

uint16_t  adConversions[N_AD_MUX_PINS + 1] = { 0, 0, 0, 0 };

static byte nextPort = 0;
static bool highCP = false;

uint16_t conversions = 0;

void initAD()
{
  // We set up the AD converter manually to have it run in the background
  ADCSRA = orBits(ADEN, ADIE, ADPS2);
  ADMUX = ADREF | 1;  // Cannot be 0 (CP pin) due to interrupt
  DIDR0 = orBits(PIN_PP_MUX, PIN_LOCK_SENSOR_MUX);    // PIN_CP_MUX excluded from having digital input disabled as we need interrupts on edges
}

void startAD(bool highCPnext)
{
  PCMSK1 = orBits(); // Disallow interrupt on CP changes during AD as it seems to trigger interrupt

  nextPort = 0;
  ADMUX = ADREF | nextPort; // First port 0
  highCP = highCPnext;
  ADCSRA |= _BV(ADSC); // Start conversion
  ++conversions;
  ++nextPort;
  ADMUX = ADREF | nextPort; // Set ADMUX in advance for next convertion to let it settle better
  PCMSK1 = orBits(PCINT8); // Allow interrupt on CP changes again as MUX is not set to CP pin any more
 }

ISR(ADC_vect)
{
  // Latest AD conversion done.
  ADCSRA &= ~_BV(ADIF); // Reset interrupt flag
  adConversions[nextPort-1+(highCP?0:1)] = ADCW;

  if (!highCP && nextPort != N_AD_MUX_PINS) {
    ADCSRA |= _BV(ADSC); // Start conversion
    ++nextPort;
    ADMUX = ADREF | nextPort;
  }
  else {
    nextPort = 0;
  }
  highCP = false;
}
