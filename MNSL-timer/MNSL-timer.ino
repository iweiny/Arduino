/*
 * Read a temperature sensor and light sensor and transmit that data via ZigBee
 * to another unit with an LCD screen
 */

#include <LiquidCrystal.h>


// Analog Pin
int keypad_in = 0;

/*
int ledPin = 13;
int temp_pin = 1;
float g_temp_C = 0;
*/

// rs (LCD pin 4) to Arduino pin 12
// rw (LCD pin 5) to Arduino pin 11
// enable (LCD pin 6) to Arduino pin 10
// LCD pin 15 to Arduino pin 13
// LCD pins d4, d5, d6, d7 to Arduino pins 5, 4, 3, 2
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

int backLight = 13;

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(9600);

	pinMode(backLight, OUTPUT);
	digitalWrite(backLight, HIGH);
	lcd.begin(16,2);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Welcome MSNL!");
	lcd.setCursor(0,1);
	lcd.print("Booting...");
	delay(1000);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Time: ");
	lcd.setCursor(0,1);
	lcd.print("Count: 0");
}

/*
 * Each value must be repeated for 3 cycles to be registered as "good"
 */
int key_read1 = -1;
int key_read2 = -1;
int pressed_key = -1;

/*
 * This function could be cleaned up quite a bit but it works.
 */
int read_keypad()
{
	int key;
	float key_value;
	key_value = analogRead(keypad_in);

	if (key_value < 965) {
		if (pressed_key >= 0)
			return -1;

		if (930 < key_value && key_value < 950) {
			key = 3;
			//Serial.print("   3");
		} else if (850 < key_value && key_value < 870) {
			key = 6;
			//Serial.print("   6");
		} else if (775 < key_value && key_value < 795) {
			key = 9;
			//Serial.print("   9");
		} else if (690 < key_value && key_value < 730) {
			key = 11;
			//Serial.print("   #");
		} else if (610 < key_value && key_value < 650) {
			key = 2;
			//Serial.print("   2");
		} else if (540 < key_value && key_value < 580) {
			key = 5;
			//Serial.print("   5");
		} else if (455 < key_value && key_value < 495) {
			key = 8;
			//Serial.print("   8");
		} else if (375 < key_value && key_value < 415) {
			key = 0;
			//Serial.print("   0");
		} else if (285 < key_value && key_value < 325) {
			key = 1;
			//Serial.print("   1");
		} else if (190 < key_value && key_value < 230) {
			key = 4;
			//Serial.print("   4");
		} else if ( 90 < key_value && key_value <  130) {
			key = 7;
			//Serial.print("   7");
		} else if ( 0 <= key_value && key_value <  30) {
			key = 10;
			//Serial.print("   *");
		}

		//Serial.print("   (");
		//Serial.print(key_value);
		//Serial.print(")\n");

		if (key_read1 == -1) {
			key_read1 = key;
		} else if (key_read1 == key && key_read2 == -1) {
			key_read2 = key;
		} else if (key_read1 == key_read2 && key_read2 == key) {
			pressed_key = key;
			key_read1 = -1;
			key_read2 = -1;
			return key;
		} else {
			key_read1 = -1;
			key_read2 = -1;
		}
	} else {
		pressed_key = -1;
	}
	return -1;
}

int cursor_pos = 0;
void enter_time(int key)
{
	int pos = (cursor_pos & 0x1) + 6;
	lcd.setCursor(pos,0);
	lcd.print(key);
	cursor_pos++;
}

void loop()
{
	int key = read_keypad();
	if (key >= 0) {
		if (key == 10) {
			Serial.print("*");
			lcd.print("*");
		} else if (key == 11) {
			Serial.print("#");
			lcd.print("#");
		} else {
			Serial.print(key);
			enter_time(key);
		}
	}
}

