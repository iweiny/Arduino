/*
 * Interface for the MNSL clock state machine
 */

#ifndef __MNSL_CLOCK_H__
#define __MNSL_CLOCK_H__

#define CLOCK_STOPPED 0
#define CLOCK_RUNNING 1
#define CLOCK_EXPIRED 2

void mnsl_clock_init();

int mnsl_clock_is_running();
void mnsl_clock_start(int time);
void mnsl_clock_stop(void);
unsigned mnsl_clock_get_time(void);

/*
 * Run the clock state machine
 * check for CLOCK_EXPIRED
 */
int mnsl_clock_run(void);

#endif /* __MNSL_CLOCK_H__ */

