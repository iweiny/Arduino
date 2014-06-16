/*
 * Interface for the general timer function of the MNSL clock
 */

#ifndef __SEQ_TIMER_H__
#define __SEQ_TIMER_H__

#include <LiquidCrystal.h>

void st_init(LiquidCrystal *lcd);

#define ST_SEQ_PPC_7  0
#define ST_SEQ_PPC_25 1
#define ST_SEQ_TYRO   2
void st_start_seq(int sequence);
void st_clock_expired(void);

void st_process_button(int button);
void st_process_keypad(int key);
void st_show_display(void);
void st_refresh_display(void);

#endif /* __SEQ_TIMER_H__ */

