/*
 * Interface for the menu of MSNL timer
 */

#include "MNSLtimer.h"

#include "menu.h"

static LiquidCrystal *display = NULL;
static setmode_cb_t setmode_cb;
static int mode_idx = 0;

struct mode {
	int mode_val;
	char *name;
};
struct mode modes[] = {
	{ MENU_GENERAL_TIMER, "General Timer" },
	{ MENU_SEQ_PPC_7, "PPC 7 yard" },
	{ MENU_SEQ_PPC_25, "PPC 25 yard" },
	{ MENU_SEQ_TYRO, "Tyro" },
};
#define NUM_MODES (sizeof(modes) / sizeof(mode))

void menu_init(LiquidCrystal *lcd, unsigned start_mode, setmode_cb_t setmode)
{
	int i;
	display = lcd;
	setmode_cb = setmode;
	mode_idx = 0;

	for (i = 0; i < NUM_MODES; i++) {
		if (modes[i].mode_val == start_mode) {
			mode_idx = i;
			break;
		}
	}
}

static void print_mode_str(void)
{
	display->setCursor(0,0);
	display->print("                     ");
	display->setCursor(0,0);
	display->print(modes[mode_idx].name);
}

static void rotate_mode_up(void)
{
	mode_idx--;
	if (mode_idx < 0)
		mode_idx = NUM_MODES-1;
	print_mode_str();
}

static void rotate_mode_down(void)
{
	mode_idx++;
	if (mode_idx >= NUM_MODES)
		mode_idx = 0;
	print_mode_str();
}

void menu_process_button(int button)
{
	switch (button) {
		case WL_A_BUTTON:
			setmode_cb(modes[mode_idx].mode_val);
			break;
		case WL_B_BUTTON:
			rotate_mode_up();
			break;
		case WL_D_BUTTON:
			rotate_mode_down();
			break;
		default:
			break;
	}
}

void menu_process_keypad(int key)
{
	serial_print("menu process keyboard ");
	serial_println(key);

	switch (key) {
		case 0: /* cancel */
			setmode_cb(MENU_CANCEL);
			break;
		case KEYPAD_POUND: /* enter */
			setmode_cb(modes[mode_idx].mode_val);
			break;
		case 3:
			rotate_mode_up();
			break;
		case 9:
			rotate_mode_down();
			break;
		default:
			break;
	}
}

void menu_show_display(void)
{
	display->clear();
	print_mode_str();
	display->setCursor(0,1);
	display->print("3/9 sel : # ent");
}

void menu_refresh_display(void)
{
	/* do nothing */
}

