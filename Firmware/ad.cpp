// A/D conversions for pin measurements are handled here

#include "definitions.h"

#define CP12VMAX  830   // 13V (probably 792)
#define CP12VMIN  713   // 10.5V
#define CP9VMAX   712   // 10.5V
#define CP9VMIN   618   // 7.5V
#define CP6VMAX   617   // 7.5V
#define CP6VMIN   523   // 4.5V
#define CPN12VMAX 48    // -10.5V
#define CPN12VMIN 0     // -12V

#define PPNCMIN   733
#define PP13AMAX  673  // 1k5
#define PP13AMIN  607
#define PP20AMAX  595  // 680
#define PP20AMIN  529
#define PP32AMAX  505  // 220
#define PP32AMIN  439

#define S1LCKMAX  734    // 10k
#define S1LCKMIN  628
#define S1UNLMAX  526    // 1k
#define S1UNLMIN  460

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
  ADMUX = ADREF | (nextPort+1); // Set ADMUX in advance for next convertion to let it settle better
  PCMSK1 = orBits(PCINT8); // Allow interrupt on CP changes again as MUX is not set to CP pin any more
 }

ISR(ADC_vect)
{
  // Latest AD conversion done.
  ADCSRA &= ~_BV(ADIF); // Reset interrupt flag
  adConversions[(nextPort!=0)?nextPort+1:(highCP?0:1)] = ADCW;

  ++nextPort;
  if (!highCP && nextPort != N_AD_MUX_PINS) {
    ADCSRA |= _BV(ADSC); // Start conversion
    ADMUX = ADREF | (nextPort+1);
  }
  else {
    nextPort = 0;
    if (!highCP) {
      // Got a complete set of measurements - set states

      // CP:
      uint16_t val = adConversions[ADC_LOWCP];
      if (val <= CPN12VMAX) {
        uint16_t highCP = adConversions[ADC_HIGHCP];
        if (highCP >= CP9VMIN && highCP <= CP9VMAX)
          cpState.set(CP::pwm9);
        else if (highCP >= CP6VMIN && highCP <= CP6VMAX)
          cpState.set(CP::pwm6);
        else
          cpState.set(CP::invalid);
      } else
        cpState.set(CP::invalid);

      // PP:
      val = adConversions[ADC_PP];
      if (val >= PP13AMIN && val <= PP13AMAX)
        ppState.set(PP::max13A);
      else if (val >= PP20AMIN && val <= PP20AMAX)
        ppState.set(PP::max20A);
      else if (val >= PP32AMIN && val <= PP32AMAX)
        ppState.set(PP::max32A);
      else
        ppState.set(PP::invalid);

      // S1:
      val = adConversions[ADC_S1];
      if (val >= S1LCKMIN && val <= S1LCKMAX)
        s1State.set(S1::locked);
      else if (val >= S1UNLMIN && val <= S1UNLMAX)
        s1State.set(S1::unlocked);
      else
        s1State.set(S1::invalid);

      // Switch:
      if (PINC & _BV(PINC3))    // Same as digitalRead(PIN_UNLOCK_SWITCH) but faster
        swState.set(SW::pressed);
      else
        swState.set(SW::invalid);
    }
  }
}
