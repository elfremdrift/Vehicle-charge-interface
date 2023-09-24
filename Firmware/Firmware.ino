/**
 * Main application for vehicle charge interface based on Type 2/Mennekes charging standard IEC 62196-2
 * Code is written to run on Atmel ATMEGA 328P hardware such as the Controllino Mini.
**/

#define MAIN
#include "definitions.h"

void writePGM(PGM_P p)
{
  char c;
  while (c = pgm_read_byte(p++)) {
    Serial.write(c);
  }
}


void setup() {
  Serial.begin(38400);

  writePGM(PSTR("Veichle charging interface initializing..."));

  pinMode(PIN_CP, INPUT);
  pinMode(PIN_PP, INPUT);
  pinMode(PIN_LOCK_SENSOR, INPUT);
  pinMode(PIN_UNLOCK_SWITCH, INPUT);

  pinMode(PIN_LOCK_OPEN, OUTPUT);         digitalWrite(PIN_LOCK_OPEN, LOW);
  pinMode(PIN_LOCK_CLOSE, OUTPUT);        digitalWrite(PIN_LOCK_CLOSE, LOW);
  pinMode(PIN_CP_PULLDOWN, OUTPUT);       digitalWrite(PIN_CP_PULLDOWN, LOW);
  pinMode(PIN_TRACTION_DISABLE, OUTPUT);  digitalWrite(PIN_TRACTION_DISABLE, LOW);

  pinMode(PIN_LED_RED, OUTPUT);           digitalWrite(PIN_LED_RED, LOW);
  pinMode(PIN_LED_YELLOW, OUTPUT);        digitalWrite(PIN_LED_YELLOW, LOW);
  pinMode(PIN_LED_GREEN, OUTPUT);         digitalWrite(PIN_LED_GREEN, LOW);

  initTimers();
  initAD();
  initState();

  writePGM(PSTR("done.\n"));
  dumpState();
}


void loop() {
  updateState();
  delay(100);
/*
  char str[128];
  sprintf(str, "%u conversions: PWM high: %u, PWM low: %u, PP: %u, S1: %u, PWM%%: %u, switch: %d\n",
            conversions, adConversions[0], adConversions[1], adConversions[2], adConversions[3], (uint16_t)(pwmValue / PWM_DIVISOR),
            digitalRead(PIN_UNLOCK_SWITCH));
  Serial.write(str);
*/

}
