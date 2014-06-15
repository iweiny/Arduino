/*
 * Interface for the menu functions of the MNSL clock
 */

#ifndef __MENU_H__
#define __MENU_H__

#include <LiquidCrystal.h>

/* Menu Timer Modes */
#define MENU_CANCEL        0
#define MENU_GENERAL_TIMER 1
#define MENU_SEQ_PPC_7     2
#define MENU_SEQ_PPC_25    3
#define MENU_SEQ_TYRO      4
typedef void (*setmode_cb_t)(int mode);

void menu_init(LiquidCrystal *lcd, unsigned start_mode, setmode_cb_t setmode);
void menu_process_button(int button);
void menu_process_keypad(int key);
void menu_show_display(void);
void menu_refresh_display(void);

#endif /* __MENU_H__ */

