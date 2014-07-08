/*
 * Interface for the general timer function of the MNSL clock
 */

#include "MNSLtimer.h"

#include "mnsl_clock.h"
#include "seq_timer.h"

struct seq_entry {
	int time;
	char *name;
};
static struct seq_entry seq_ppc7[] = {
	{ 10, "1st 7 yard" },
	{ 10, "2nd 7 yard" },
	{ 15, "1st 15 yard" },
	{ 15, "2nd 15 yard" },
	{ 30, "3rd 15 yard" },
	{ 30, "4th 15 yard" },
	{ 60, "1st 25 yard" },
	{ 60, "2nd 25 yard" },
};
#define NUM_PPC7_ENTRY (sizeof(seq_ppc7) / sizeof(struct seq_entry))

static struct seq_entry seq_ppc25[] = {
	{ 60, "1st 25 yard" },
	{ 60, "2nd 25 yard" },
	{ 30, "1st 15 yard" },
	{ 30, "2nd 15 yard" },
	{ 15, "3rd 15 yard" },
	{ 15, "4th 15 yard" },
	{ 10, "1st 7 yard" },
	{ 10, "2nd 7 yard" },
};
#define NUM_PPC25_ENTRY (sizeof(seq_ppc25) / sizeof(struct seq_entry))

static struct seq_entry seq_tyro[] = {
	{ 5, "1st lower" },
	{ 5, "2nd lower" },
	{ 5, "3rd lower" },
	{ 10, "All lower" },
	{ 20, "Lower, rel, low" },
	{ 5, "1st upper" },
	{ 5, "2nd upper" },
	{ 5, "3rd upper" },
	{ 10, "All upper" },
	{ 20, "Upper, rel, up" },
};
#define NUM_TYRO_ENTRY (sizeof(seq_tyro) / sizeof(struct seq_entry))

struct sequence {
	int               mode;
	unsigned          idx;
	unsigned          max_idx;
	struct seq_entry *seq;
};

static LiquidCrystal *display = NULL;
static sequence cur_seq = {
	ST_SEQ_PPC_7,
	0,
	NUM_PPC7_ENTRY,
	seq_ppc7
};

static int get_cur_seq_time(void)
{
	return (cur_seq.seq[cur_seq.idx].time);
}

static char *get_cur_seq_str_name(void)
{
	return (cur_seq.seq[cur_seq.idx].name);
}

static void inc_seq(void)
{
	cur_seq.idx++;
	if (cur_seq.idx >= cur_seq.max_idx)
		cur_seq.idx = 0;
}

static char *get_cur_seq_name(void)
{
	switch (cur_seq.mode) {
		case ST_SEQ_PPC_7:
			return ("PPC 7 :");
		case ST_SEQ_PPC_25:
			return ("PPC 25:");
		case ST_SEQ_TYRO:
			return ("Tyro  :");
	}
	return ("Time  :");
}

void st_init(LiquidCrystal *lcd)
{
	display = lcd;
}

void st_start_seq(int sequence)
{
	switch (sequence) {
		case ST_SEQ_PPC_7:
			cur_seq.seq = seq_ppc7;
			cur_seq.max_idx = NUM_PPC7_ENTRY;
			break;
		case ST_SEQ_PPC_25:
			cur_seq.seq = seq_ppc25;
			cur_seq.max_idx = NUM_PPC25_ENTRY;
			break;
		case ST_SEQ_TYRO:
			cur_seq.seq = seq_tyro;
			cur_seq.max_idx = NUM_TYRO_ENTRY;
			break;
		default:
			return;
	}
	cur_seq.mode = sequence;
	cur_seq.idx = 0;
}

static void display_str_time(void)
{
	int time = get_cur_seq_time();
	display->setCursor(12,0);
	if (time < 10) {
		display->print(0);
	}
	display->print(time);
}

static void display_mode(void)
{
	display->setCursor(0,0);
	display->print(get_cur_seq_name());
}

static void display_str_name(void)
{
	display->setCursor(0,1);
	display->print("                       ");
	display->setCursor(0,1);
	display->print(get_cur_seq_str_name());
}

static void display_clock_time (void)
{
	unsigned time = mnsl_clock_get_time();
	display->setCursor(9,0);
	if (time < 10) {
		display->print(0);
	}
	display->print(time);
}

static void start_timer(void)
{
	mnsl_clock_start(get_cur_seq_time());
	display_clock_time();
}

static void stop_timer(void)
{
	mnsl_clock_stop();
	display_clock_time();
	inc_seq();
	display_str_time();
	display_str_name();
}

/** =========================================================================
 * process start and cancel button presses.
 */
static void do_start_button(void)
{
	sound_buzzer(500);
	if (mnsl_clock_is_running())
		stop_timer();
	else
		start_timer();
}

void st_process_button(int button)
{
	switch (button) {
		case WL_A_BUTTON:
			do_start_button();
			break;
		case WL_D_BUTTON:
			stop_timer();
			break;
	}
}

void st_process_keypad(int key)
{
	/* do nothing for now */
}

void st_show_display(void)
{
	display->clear();
	display->setCursor(0,0);
	display->print("Time  :  00/00 s");
	display_mode();
	display_str_time();
	display_str_name();
}

void st_refresh_display(void)
{
	display_clock_time();
}

void st_clock_expired(void)
{
	inc_seq();
	display_str_time();
	display_str_name();
}
