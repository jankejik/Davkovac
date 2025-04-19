/*! \file Dsp_Object.h
    \author Jan Kejík
*/

#include "stddef.h"
#include "stdint.h"
#include "Param.h"
#include "Arduino.h"
#include "Common.h"


#ifndef __DSP_OBJECT_H_
#define __DSP_OBJECT_H_

#define NEX_RET_CMD_FINISHED            (0x01)

#define AUTO_REPEAT_START 1000 /* Milisekund */
#define AUTO_REPEAT_SPEED 100  /* Milisekund */
#define INVALID_BUTT 255
#define INVALID_PAGE 255
#define BUTT_ON_EVENT 1
#define BUTT_OFF_EVENT 0

typedef enum
{
    evnt_None,
    evnt_Add,
    evnt_Sub,
    evnt_Set,
    evnt_Get,
    evnt_Set_Page,
    evnt_Disp,
    evnt_Toggle,
    evnt_Moment,
    evnt_Wave,

} Event_Action;

typedef struct
{
    Event_Action evea;
    uint8_t butt_id;
    param *dispParam;
    int32_t setVal;
    const char disp_id[15];

} Event_Action_Def;

class Dsp_Object
{
private:
    static uint32_t event_time;
    static const Event_Action_Def *pcfg_act;
    static bool auto_repeat;
    static bool Param_Change(const Event_Action_Def *pcfg);
    static uint16_t responseTimeout;

public:
    static bool Process_Event(const Event_Action_Def *pcfg, uint8_t butt_id, uint32_t event);
    static bool AutoRepeat(void);
    static bool Value_Send(const Event_Action_Def *pcfg);
    static bool Set_Page(uint8_t page);
    static bool Value_Get(int32_t val);
    static bool Is_Page_Change(uint8_t * page);
    static void SendCommand(const char* cmd);
    static const Event_Action_Def *pcfgSent;
    static bool Value_Set(void);
    static bool Value_Send_All(const Event_Action_Def *pcfg);
    static uint8_t pageAct;
    static void Value_Error(void);
    static void Transparent_Data_Send(void);
    static bool recvRetCommandFinished(uint32_t timeout = 100);


};
#ifdef DISP_DEBUG

#define dbDispObj(txt) dbSerialPrintln(txt)

#else

#define dbDispObj(txt)

#endif


#endif //__DSP_OBJECT_H_
