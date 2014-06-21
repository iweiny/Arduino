/*
 * Global defines for MNSLtimer.ino
 */

#ifndef __MNSL_TIMER_H__
#define __MNSL_TIMER_H__

#define SERIAL_DEBUG 1
#include "serial_debug.h"

/* Button defines */
#define NO_BUTTON      0
#define START_BUTTON   1
#define CANCEL_BUTTON  2
#define WL_B_BUTTON    3
#define WL_C_BUTTON    4

/* Keypad defines */
#define KEYPAD_STAR    10
#define KEYPAD_POUND   11

/* global functions */
void sound_buzzer(void);

#endif /* __MNSL_TIMER_H__ */
