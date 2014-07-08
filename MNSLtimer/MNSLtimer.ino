/*
 * MNSL timer project
 */

#include <Time.h>
#include <LiquidCrystal.h>

#include "MNSLtimer.h"
#include "mnsl_clock.h"
#include "menu.h"
#include "gen_timer.h"
#include "seq_timer.h"

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
int wl_A_button        = 9;
int wl_B_button        = A3;
int wl_C_button        = A4;
int wl_D_button        = 8;


/**
 * Gobals
 */
#define TM_MENU           0
#define TM_GENERAL_TIMER  1
#define TM_SEQUENCE_TIMER 2
int timer_mode = TM_GENERAL_TIMER;
int old_timer_mode = timer_mode;

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
	int wl_a_but = digitalRead(wl_A_button);
	int wl_b_but = digitalRead(wl_B_button);
	int wl_c_but = digitalRead(wl_C_button);
	int wl_d_but = digitalRead(wl_D_button);

	if (wl_a_but == HIGH) {
		if (but_held) {
			return NO_BUTTON;
		}
		but_held = 1;
		return WL_A_BUTTON;
	} else if (wl_b_but == HIGH) {
		if (but_held) {
			return NO_BUTTON;
		}
		but_held = 1;
		return WL_B_BUTTON;
	} else if (wl_c_but == HIGH) {
		if (but_held) {
			return NO_BUTTON;
		}
		but_held = 1;
		return WL_C_BUTTON;
	} else if (wl_d_but == HIGH) {
		if (but_held) {
			return NO_BUTTON;
		}
		but_held = 1;
		return WL_D_BUTTON;
	} else {
		but_held = 0;
	}
	return NO_BUTTON;
}

/** =========================================================================
 * General buzzer function for the temporary buzzer we have wired up.
 * Sound a C note for ~ .5 seconds
 */
void sound_c_note(void)
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
 * Assert the buzzer line to the buzzer box
 */
void sound_buzzer(int ms)
{
	digitalWrite(buzzer, HIGH);
	delay(ms);
	digitalWrite(buzzer, LOW);
}

/** =========================================================================
 * refresh display based on timer mode
 */
void refresh_display(void)
{
	switch (timer_mode) {
		case TM_MENU:
			menu_refresh_display();
			break;
		case TM_GENERAL_TIMER:
			gt_refresh_display();
			break;
		case TM_SEQUENCE_TIMER:
			st_refresh_display();
			break;
		default:
			break;
	}
}

/** =========================================================================
 * refresh display based on timer mode
 */
void show_display(void)
{
	switch (timer_mode) {
		case TM_MENU:
			menu_show_display();
			break;
		case TM_GENERAL_TIMER:
			gt_show_display();
			break;
		case TM_SEQUENCE_TIMER:
			st_show_display();
			break;
		default:
			break;
	}
}

/** =========================================================================
 * refresh display based on timer mode
 */
void clock_expired(void)
{
	switch (timer_mode) {
		case TM_SEQUENCE_TIMER:
			st_clock_expired();
			break;
		default:
			break;
	}
}

/** =========================================================================
 * Set mode callback called by menu module when the user selects a mode.
 */
void setmode(int mode)
{
	switch (mode) {
		case MENU_CANCEL:
			timer_mode = old_timer_mode;
			break;
		case MENU_GENERAL_TIMER:
			timer_mode = TM_GENERAL_TIMER;
			break;
		case MENU_SEQ_PPC_7:
			timer_mode = TM_SEQUENCE_TIMER;
			st_start_seq(ST_SEQ_PPC_7);
			break;
		case MENU_SEQ_PPC_25:
			timer_mode = TM_SEQUENCE_TIMER;
			st_start_seq(ST_SEQ_PPC_25);
			break;
		case MENU_SEQ_TYRO:
			timer_mode = TM_SEQUENCE_TIMER;
			st_start_seq(ST_SEQ_TYRO);
			break;
	}

	serial_print("new mode selected ");
	serial_println(timer_mode);

	show_display();
}

/** =========================================================================
 * Main setup and event processing
 */
void splash_screen()
{
	lcd.begin(16,2);
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Welcome MSNL!");
	lcd.setCursor(0,1);
	lcd.print("Booting...");

	// As if we have anything to do here...  ;-)
	delay(1000);
}

void setup()
{
	// Open serial communications and wait for port to open:
	setup_print(9600);
	pinMode(wl_B_button, INPUT);
	pinMode(wl_C_button, INPUT);

	init_backlight();
	backlight_on();

	pinMode(buzzer, OUTPUT);

	splash_screen();

	mnsl_clock_init();
	gt_init(&lcd);
	menu_init(&lcd, MENU_GENERAL_TIMER, setmode);

	timer_mode = TM_GENERAL_TIMER;
	gt_show_display();
	st_init(&lcd);
}

void enter_menu_mode(void)
{
	if (timer_mode != TM_MENU)
		old_timer_mode = timer_mode;
	timer_mode = TM_MENU;
	show_display();
}

void process_keypad()
{
	int key = read_keypad();
	if (key >= 0) {
		backlight_on();
		if (key == KEYPAD_STAR && !mnsl_clock_is_running()) {
			enter_menu_mode();
			return;
		}
		switch (timer_mode) {
			case TM_MENU:
				menu_process_keypad(key);
				break;
			case TM_GENERAL_TIMER:
				gt_process_keypad(key);
				break;
			case TM_SEQUENCE_TIMER:
				st_process_keypad(key);
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
		if (button == WL_C_BUTTON && !mnsl_clock_is_running()) {
			enter_menu_mode();
			return;
		}
		switch (timer_mode) {
			case TM_MENU:
				menu_process_button(button);
				break;
			case TM_GENERAL_TIMER:
				gt_process_button(button);
				break;
			case TM_SEQUENCE_TIMER:
				st_process_button(button);
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
		sound_buzzer(1750);
		mnsl_clock_stop();
		clock_expired();
	}
	refresh_display();
}
