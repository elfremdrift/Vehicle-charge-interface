/**
 * Main application for vehicle charge interface based on Type 2/Mennekes charging standard IEC 62196-2
 * Code is written to run on Atmel ATMEGA 328P hardware such as the Controllino Mini.
**/

#include "definitions.h"

void setup() {
  pinMode(PIN_CP, INPUT);
  pinMode(PIN_PP, INPUT);
  pinMode(PIN_LOCK_SENSOR, INPUT);
  pinMode(PIN_UNLOCK_SWITCH, INPUT);

  pinMode(PIN_LOCK_OPEN, OUTPUT);         digitalWrite(PIN_LOCK_OPEN, LOW);
  pinMode(PIN_LOCK_CLOSE, OUTPUT);        digitalWrite(PIN_LOCK_CLOSE, LOW);
  pinMode(PIN_CP_PULLDOWN, OUTPUT);       digitalWrite(PIN_CP_PULLDOWN, LOW);
  pinMode(PIN_TRACTION_DISABLE, OUTPUT);  digitalWrite(PIN_TRACTION_DISABLE, LOW);

  Serial.begin(9600);
  Serial.write("\nHey there\n");

  Serial.print("Pin A0: "); Serial.println(A0);
  Serial.print("Pin A1: "); Serial.println(PIN_PP);
  Serial.print("Pin A2: "); Serial.println(PIN_LOCK_SENSOR);
  Serial.print("Pin A3: "); Serial.println(PIN_UNLOCK_SWITCH);

  pinMode(PIN_LED_RED, OUTPUT);           digitalWrite(PIN_LED_RED, LOW);
  pinMode(PIN_LED_YELLOW, OUTPUT);        digitalWrite(PIN_LED_YELLOW, LOW);
  pinMode(PIN_LED_GREEN, OUTPUT);         digitalWrite(PIN_LED_GREEN, LOW);

  initAD();
  startAD();
}

extern uint16_t conversions;
extern uint16_t adConversions[];

void loop() {
  Serial.print(conversions);
  Serial.write("-Yo?  ");
  // put your main code here, to run repeatedly:
  delay(100);
  digitalWrite(PIN_LED_GREEN, LOW);
  delay(100);
  digitalWrite(PIN_LED_GREEN, HIGH);

  char str[64];
  sprintf(str, "%u conversions: %u, %u, %u - %d\n", conversions, adConversions[0], adConversions[1], adConversions[2], digitalRead(PIN_UNLOCK_SWITCH));
  Serial.write(str);
}
