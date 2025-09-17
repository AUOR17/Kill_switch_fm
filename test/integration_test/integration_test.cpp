#include <Arduino.h>
void setup() {
  Serial.begin(115200);
  while(!Serial){;}
  Serial.println("\n[INTEGRATION TEST] (vacío por ahora)");
}
void loop() { delay(100); }