/*
 * MNSL timer project
 */

#include <Time.h>
#include <LiquidCrystal.h>

#include "MNSLtimer.h"
#include "mnsl_clock.h"
#include "gen_timer.h"

// Analog Pins
int keypad_in_pin     = 0;

// rs (LCD pin 4) to Arduino pin 12
// rw (LCD pin 5) to Arduino pin 11
// enable (LCD pin 6) to Arduino pin 10
// LCD pin 15 to Arduino pin 13
// LCD pins d4, d5, d6, d7 to Arduino pins 5, 4, 3, 2
LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2);

// Pin definitions
int buzzer             = 6;
int lcd_back_light_pin = 7;
int cancel_button      = 8;
int start_stop_button  = 9;


/**
 * Gobals
 */
#define TM_MENU           0
#define TM_GENERAL_TIMER  1
#define TM_SEQUENCE_TIMER 2
int timer_mode = TM_GENERAL_TIMER;

/** =========================================================================
 * Process the backlight functionality
 */
void backlight_off()
{
	digitalWrite(lcd_back_light_pin, LOW);
}
void backlight_on()
{
	digitalWrite(lcd_back_light_pin, HIGH);
}
time_t backlight_on_at;
void timeout_backlight()
{
	time_t cur_time = now();
	if (cur_time - backlight_on_at > 5 && !mnsl_clock_is_running()) {
		backlight_off();
		backlight_on_at = cur_time;
	}
}
void init_backlight()
{
	pinMode(lcd_back_light_pin, OUTPUT);
	backlight_on_at = now();
}

/** =========================================================================
 * Read Keypad
 */
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
			serial_print("   3");
		} else if (850 < key_value && key_value < 870) {
			key = 6;
			serial_print("   6");
		} else if (775 < key_value && key_value < 795) {
			key = 9;
			serial_print("   9");
		} else if (690 < key_value && key_value < 730) {
			key = 11;
			serial_print("   #");
		} else if (610 < key_value && key_value < 650) {
			key = 2;
			serial_print("   2");
		} else if (540 < key_value && key_value < 580) {
			key = 5;
			serial_print("   5");
		} else if (455 < key_value && key_value < 495) {
			key = 8;
			serial_print("   8");
		} else if (375 < key_value && key_value < 415) {
			key = 0;
			serial_print("   0");
		} else if (285 < key_value && key_value < 325) {
			key = 1;
			serial_print("   1");
		} else if (190 < key_value && key_value < 230) {
			key = 4;
			serial_print("   4");
		} else if ( 90 < key_value && key_value <  130) {
			key = 7;
			serial_print("   7");
		} else if ( 0 <= key_value && key_value <  30) {
			key = 10;
			serial_print("   *");
		}

		serial_print("   (");
		serial_print(key_value);
		serial_print(")\n");

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


/** =========================================================================
 * Read the buttons presses which we have
 */
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

/** =========================================================================
 * General buzzer function for the temporary buzzer we have wired up.
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


/** =========================================================================
 * refresh display based on timer mode
 */
void refresh_display(void)
{
	switch (timer_mode) {
		case TM_GENERAL_TIMER:
			gt_refresh_display();
			break;
		default:
			break;
	}
}

/** =========================================================================
 * Main setup and event processing
 */
void setup()
{
	// Open serial communications and wait for port to open:
	setup_print(9600);

	init_backlight();
	backlight_on();

	pinMode(buzzer, OUTPUT);
	lcd.begin(16,2);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Welcome MSNL!");
	lcd.setCursor(0,1);
	lcd.print("Booting...");

	// As if we have anything to do here...  ;-)
	delay(1000);

	timer_mode = TM_GENERAL_TIMER;
	gt_init(&lcd);

	mnsl_clock_init();
}

void process_keypad()
{
	int key = read_keypad();
	if (key >= 0) {
		backlight_on();
		switch (timer_mode) {
			case TM_GENERAL_TIMER:
				gt_process_keypad(key);
				break;
			default:
				break;
		}
	}
}

void process_buttons()
{
	int button = read_buttons();
	if (button != NO_BUTTON) {
		backlight_on();
		switch (timer_mode) {
			case TM_GENERAL_TIMER:
				gt_process_button(button);
				break;
			default:
				break;
		}
	}
}

void loop()
{
	process_keypad();
	process_buttons();
	//timeout_backlight();
	if (mnsl_clock_run() == CLOCK_EXPIRED) {
		sound_buzzer();
		mnsl_clock_stop();
	}
	refresh_display();
}
