/*
 * Read the data comming from the temp/light sensor and write to the LCD screen
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>


/**
 * NOTE: LCD pins
 *
 * MEGA: sda -> sda
 *       scl -> scl
 *
 */
LiquidCrystal_I2C lcd(0x3f,20,4);
int ledPin = 13;
String temp;
String light;

void print_labels()
{
	lcd.clear();

	lcd.setCursor(0,0);
	lcd.print("Temp  : ");
	lcd.print(temp);

	lcd.setCursor(0,2);
	lcd.print("Light : ");
	lcd.print(light);
}

void setup()
{
	lcd.init();
	lcd.init();
	lcd.backlight();
	lcd.setCursor(0,0);
	lcd.print("Starting...");
	Serial.begin(9600);
	delay(500);
	print_labels();
}


#define MSG_SIZE 32
char msg[MSG_SIZE];
int read_message()
{
	int in_byte = 0;
	int i = 0;
	int rc = 0;
	if (Serial.available() > 0) {
		// wait for the chars to get buffered up
		delay(250);
		rc = 1;
		do {
			in_byte = Serial.read();
			msg[i] = in_byte;
			i++;
		} while (in_byte != ';' && i < MSG_SIZE-1);
		msg[i-1] = '\0';
	}
	return (rc);
}

void decode_message()
{
	if (msg[0] == 'T') {
		temp = &msg[1];
	}
	if (msg[0] == 'L') {
		light = &msg[1];
	}
}

void print_message()
{
	lcd.setCursor(9,0);
	lcd.print(temp);
	lcd.print("      ");

	lcd.setCursor(9,2);
	lcd.print(light);
	lcd.print("      ");
}

void loop()
{
	if (read_message()) {
		decode_message();
		print_message();
	}
}

