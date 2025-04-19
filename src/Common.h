
/*! \file Display.h
    \author Jan Kejík
*/

#ifndef __COMMON_H_
#define __COMMON_H_
#define MSEC_PER_MIN 60000UL
#define SEC_PER_MIN 60UL
#define MIN_PER_HOUR 60UL
#define HOUR_PER_DAY 24UL
#define PUMP_CAPACITY 39 // mil/min
#define UNIT_POOL_VOL_M3 0.1
#define UNIT_PH 0.1

#define VERZE_MAJOR 1
#define VERZE_MINOR 10
#define CMP_DATE __DATE__


#define dbSerial Serial
#define DEBUG_SERIAL_ENABLE
/**
 * Define nexSerial for communicate with Nextion touch panel. 
 */
#define nexSerial Serial1

#ifdef DEBUG_SERIAL_ENABLE
#define dbSerialPrint(a)    dbSerial.print(a)
#define dbSerialPrintln(a)  dbSerial.println(a)
#define dbSerialBegin(a)    dbSerial.begin(a)
#else
#define dbSerialPrint(a)    do{}while(0)
#define dbSerialPrintln(a)  do{}while(0)
#define dbSerialBegin(a)    do{}while(0)
#endif

//#define ORP_DEBUG

#define WEB_PARAM_DEBUG
//#define ORP_DEBUG
#define ANA_DEBUG
//#define DISP_DEBUG
//#define DISPLAY_DEBUG

#endif //__COMMON_H_