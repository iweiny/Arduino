/*
 * Implements the MNSL clock state machine
 */

#include <Time.h>

#include "mnsl_clock.h"

time_t prev_time;
int clock_mode = CLOCK_STOPPED;
int countdown_time = 0;

void mnsl_clock_init()
{
	clock_mode = CLOCK_STOPPED;
	prev_time = now();
}

int mnsl_clock_is_running()
{
	return (clock_mode == CLOCK_RUNNING);
}

static void timer_func(void)
{
	if (!mnsl_clock_is_running())
		return;

	countdown_time--;
	if (countdown_time <= 0) {
		clock_mode = CLOCK_EXPIRED;
		countdown_time = 0;
	}
}

int mnsl_clock_run(void)
{
	time_t cur_time = now();
	if (cur_time > prev_time) {
		timer_func();
		prev_time = cur_time;
	}
	return clock_mode;
}

void mnsl_clock_start(int time)
{
	countdown_time = time;
	clock_mode = CLOCK_RUNNING;
}

void mnsl_clock_stop(void)
{
	//countdown_time = 0;
	clock_mode = CLOCK_STOPPED;
}

unsigned mnsl_clock_get_time(void)
{
	if (countdown_time < 0)
		return 0;
	return countdown_time;
}

