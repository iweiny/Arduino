/*
 * Read a temperature sensor and light sensor and transmit that data via ZigBee
 * to another unit with an LCD screen
 */

#include <SPI.h>

int ledPin = 13;
int temp_pin = 1;
float g_temp_C = 0;

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(9600);

	//pinMode(ledPin, OUTPUT);
}

int light_level = 0;

void send_message()
{
	Serial.print("T");
	Serial.print(g_temp_C, 2);
	Serial.print(";");

	Serial.print("L");
	Serial.print(light_level);
	Serial.print(";");
}

int light_pin = 0;
int led_pin = 9;

void read_light_level()
{
	light_level = analogRead(light_pin);
	light_level = map(light_level, 300, 1000, 255, 0);
	light_level = constrain(light_level, 0, 255);
	analogWrite(led_pin, light_level);
}

float get_volt_mV(int pin)
{
	return (analogRead(pin) * 0.004882814);
}

void read_temperature()
{
	float temp = get_volt_mV(temp_pin);
	temp = (temp - 0.5) * 100;
	g_temp_C = temp;
}

/*
void write_message()
{
	static int toggle = 0;
	if (toggle) {
		Serial.write("Steggy;");
		toggle = 0;
		digitalWrite(ledPin,HIGH);
	} else {
		Serial.write("Ira;");
		toggle = 1;
		digitalWrite(ledPin,LOW);
	}
}
*/

void loop()
{
	read_light_level();
	read_temperature();
	send_message();
	delay(1000);
}

