/*
 * MNSL timer project
 */

#include <Time.h>
#include <LiquidCrystal.h>

// Analog Pins
int keypad_in_pin     = 0;

// rs (LCD pin 4) to Arduino pin 12
// rw (LCD pin 5) to Arduino pin 11
// enable (LCD pin 6) to Arduino pin 10
// LCD pin 15 to Arduino pin 13
// LCD pins d4, d5, d6, d7 to Arduino pins 5, 4, 3, 2
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

int buzzer             = 6;
int lcd_back_light_pin = 7;
int cancel_button      = 8;
int start_stop_button  = 9;


/**
 * Gobals for clock
 */
#define STOPPED  0
#define RUNNING  1
int clock_mode = 0;
int countdown_time = 0;
int entered_time = 0;
int string_count = 0;
int last_string_time = 0;

time_t backlight_on_at;
void checkBackLight()
{
	time_t cur_time = now();
	if (cur_time - backlight_on_at > 5 && clock_mode != RUNNING) {
		digitalWrite(lcd_back_light_pin, LOW);
		backlight_on_at = cur_time;
	}
}

/*
 * Each value must be repeated for 3 cycles to be registered as "good"
 */
int key_read1 = -1;
int key_read2 = -1;
// the key can be held don't register this.
int pressed_key = -1;

/*
 * This function could be cleaned up quite a bit but it works.
 *
 * Return the key pressed
 *     [0-9, 10, 11] = [0-9, *, #]
 * or -1 if no key has been pressed.
 */
int read_keypad()
{
	int key;
	float key_value;
	key_value = analogRead(keypad_in_pin);

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


#define NO_BUTTON      0
#define START_BUTTON   1
#define CANCEL_BUTTON  2

int but_held = 0;
int read_buttons(void)
{
	int st_but = digitalRead(start_stop_button);
	int can_but = digitalRead(cancel_button);

	if (st_but == HIGH) {
		if (but_held) {
			return NO_BUTTON;
		}
		but_held = 1;
		return START_BUTTON;
	} else if (can_but == HIGH) {
		if (but_held) {
			return NO_BUTTON;
		}
		but_held = 1;
		return CANCEL_BUTTON;
	} else {
		but_held = 0;
	}
	return NO_BUTTON;
}

void display_entered_time (void)
{
	lcd.setCursor(12,0);
	if (entered_time < 10) {
		lcd.print(0);
	}
	lcd.print(entered_time);
}

void display_countdown_time (void)
{
	lcd.setCursor(7,0);
	if (countdown_time < 10) {
		lcd.print(0);
	}
	lcd.print(countdown_time);
}

void display_string_count(void)
{
	lcd.setCursor(8, 1);
	if (string_count > 10) {
		string_count = string_count % 10;
	}
	lcd.print(string_count);
	lcd.setCursor(12, 1);
	if (entered_time < 10) {
		lcd.print(0);
	}
	lcd.print(entered_time);
}

void enter_time(int key)
{
	entered_time = entered_time % 10;
	entered_time *= 10;
	entered_time += key;
	display_entered_time();
}

/*
 * Sound a C note for ~ .5 seconds
 */
void sound_buzzer(void)
{
	int i;
	for (i = 0; i < 125; i++) {
		digitalWrite(buzzer, LOW);
		delayMicroseconds(1912);
		digitalWrite(buzzer, HIGH);
		delayMicroseconds(1912);
	}
}

void timer_func(void)
{
	if (clock_mode == STOPPED) {
		return;
	}
	countdown_time--;
	display_countdown_time();
	if (countdown_time <= 0) {
		sound_buzzer();
		clock_mode = STOPPED;
	}
}

void start_timer(void)
{
	if (entered_time <= 0) {
		return;
	}
	if (last_string_time && last_string_time == entered_time) {
		string_count++;
	} else {
		last_string_time = entered_time;
		string_count = 1;
	}
	display_string_count();
	countdown_time = entered_time;
	display_countdown_time();
	clock_mode = RUNNING;
}

void stop_timer(void)
{
	countdown_time = 0;
	display_countdown_time();
	clock_mode = STOPPED;
}

void do_start_button(void)
{
	sound_buzzer();
	if (clock_mode == STOPPED) {
		start_timer();
	} else {
		stop_timer();
	}
}

void do_cancel_button(void)
{
	stop_timer();
}

time_t prev_time;
void checkTime(void)
{
	time_t cur_time = now();
	if (cur_time > prev_time) {
		timer_func();
		prev_time = cur_time;
	}
}

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(9600);

	pinMode(lcd_back_light_pin, OUTPUT);
	digitalWrite(lcd_back_light_pin, HIGH);
	pinMode(buzzer, OUTPUT);
	lcd.begin(16,2);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Welcome MSNL!");
	lcd.setCursor(0,1);
	lcd.print("Booting...");
	// As if we have anything to do here...  ;-)
	delay(1000);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Time:  00 / 00 s");
	lcd.setCursor(0,1);
	lcd.print("Count:  0 @ 00 s");
	backlight_on_at = now();
	prev_time = now();
}

void loop()
{
	int key = read_keypad();
	if (key >= 0) {
		digitalWrite(lcd_back_light_pin, HIGH);
		if (key == 10) {
			Serial.println("keyboard *");
		} else if (key == 11) {
			Serial.println("keyboard #");
		} else {
			Serial.print("keyboard ");
			Serial.println(key);
			enter_time(key);
		}
	}
	int button = read_buttons();
	if (button != NO_BUTTON) {
		digitalWrite(lcd_back_light_pin, HIGH);
		switch (button) {
			case START_BUTTON:
				Serial.println("start...");
				do_start_button();
				break;
			case CANCEL_BUTTON:
				Serial.println("cancel...");
				do_cancel_button();
				break;
		}
	}
	//checkBackLight();
	checkTime();
}
