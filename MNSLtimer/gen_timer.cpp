/*
 * Interface for the general timer function of the MNSL clock
 */

#include "MNSLtimer.h"

#include "mnsl_clock.h"
#include "gen_timer.h"

int entered_time = 0;
int string_count = 0;
int last_string_time = 0;
LiquidCrystal *display = NULL;

/** =========================================================================
 * Display functions for General timer
 */
static void display_entered_time (void)
{
	display->setCursor(12,0);
	if (entered_time < 10) {
		display->print(0);
	}
	display->print(entered_time);
}

static void display_clock_time (void)
{
	unsigned time = mnsl_clock_get_time();
	display->setCursor(7,0);
	if (time < 10) {
		display->print(0);
	}
	display->print(time);
}

static void display_string_count(void)
{
	display->setCursor(8, 1);
	if (string_count > 10) {
		string_count = string_count % 10;
	}
	display->print(string_count);
	display->setCursor(12, 1);
	if (entered_time < 10) {
		display->print(0);
	}
	display->print(entered_time);
}

/** =========================================================================
 * process input for the general timer module
 */
static void enter_time(int key_val)
{
	if (mnsl_clock_is_running())
		return;

	entered_time = entered_time % 10;
	entered_time *= 10;
	entered_time += key_val;
	display_entered_time();
}

static void start_timer(void)
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
	display_clock_time();
	mnsl_clock_start(entered_time);
}

static void stop_timer(void)
{
	display_clock_time();
	mnsl_clock_stop();
}

/** =========================================================================
 * process start and cancel button presses.
 */
static void do_start_button(void)
{
	sound_buzzer();
	if (mnsl_clock_is_running())
		stop_timer();
	else
		start_timer();
}

static void do_cancel_button(void)
{
	stop_timer();
}


/** =========================================================================
 * Global interface
 */
void gt_init(LiquidCrystal *lcd)
{
	display = lcd;
	display->clear();
	display->setCursor(0,0);
	display->print("Time:  00 / 00 s");
	display->setCursor(0,1);
	display->print("Count:  0 @ 00 s");
}

void gt_process_button(int button)
{
	switch (button) {
		case START_BUTTON:
			serial_println("start button pressed...");
			do_start_button();
			break;
		case CANCEL_BUTTON:
			serial_println("cancel button pressed...");
			do_cancel_button();
			break;
	}
}

void gt_process_keypad(int key)
{
	if (key == KEYPAD_STAR) {
		serial_println("keyboard *");
	} else if (key == KEYPAD_POUND) {
		serial_println("keyboard #");
	} else {
		serial_print("keyboard ");
		serial_println(key);
		enter_time(key);
	}
}

void gt_refresh_display(void)
{
	display_clock_time();
}

