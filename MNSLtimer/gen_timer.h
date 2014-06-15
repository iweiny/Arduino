/*
 * Interface for the general timer function of the MNSL clock
 */

#ifndef __GEN_TIMER_H__
#define __GEN_TIMER_H__

#include <LiquidCrystal.h>

void gt_init(LiquidCrystal *lcd);
void gt_process_button(int button);
void gt_process_keypad(int key);
void gt_refresh_display(void);

#endif /* __GEN_TIMER_H__ */

