/*
 * Implement optional serial debug prints.
 */


#ifndef __SERIAL_DEBUG_H__
#define __SERIAL_DEBUG_H__

#ifdef SERIAL_DEBUG
	#define setup_print(baud) \
		Serial.begin(baud)
	#define serial_print(str) \
		Serial.print(str);
	#define serial_println(str) \
		Serial.println(str);
#else
	#define setup_print(baud)
	#define serial_print(str)
	#define serial_println(str)
#endif


#endif /* __SERIAL_DEBUG_H__ */

