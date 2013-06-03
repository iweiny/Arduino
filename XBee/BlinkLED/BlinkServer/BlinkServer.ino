/*
 * Server code writes 'H' and 'L' to the ZigBee to Blink an LED on the remote board
 * Locally blinks an LED in an opposite pattern
 */

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
}

/* Server code writes 'H' and 'L' in a loop with delay */
void loop() {
  Serial.write('H');
  digitalWrite(ledPin, LOW);
  delay(1000);
  Serial.write('L');
  digitalWrite(ledPin, HIGH);
  delay(1000);
}

