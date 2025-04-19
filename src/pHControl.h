/*! \file pHControl.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "Param.h"
#include "ChangeIcon.h"
#include "PHReading.h"
#include "MotorControl.h"
#include "Common.h"
#include "TimeBlock.h"

#ifndef __PH_CONTROL_H_
#define __PH_CONTROL_H_

#define PH_CTRL_CALL_PERIOD 5000 // milisecond
#define MAX_CTRL_ATTEMPTS 5

#define PH_CONTROL_DEATH_ZONE 2


typedef enum
{
    Ctrl_done,
    Ctrl_up,
    Ctrl_down,

} Control_t;

class pHControl
{
private:

    static uint8_t altAttempts; // Pocet pokusu jednim cinidlem pro dosazeni stanoveno pH.
    static uint8_t Attempts;    // Pocet pokusu jednim cinidlem pro dosazeni stanoveno pH.
    static Control_t lastCtrl;
    static uint32_t dose;
    static uint32_t OnTime;

    static void CalcCorection(int32_t deltapH);

public:
    static void Init(void);
    static void pHAutocontrol(void);
    static void CtrlActionUp(bool on, uint32_t time);
    static void CtrlActionDown(bool on, uint32_t time);
    static bool SteadyTime(void);
    static bool CtrlError(void);
};

class pHManualControl : public ChangeIcon 
{
    public:
    pHManualControl(int32_t a, int32_t b, int32_t c): ChangeIcon(a,b,c){}
    bool SetVal(int32_t newVal);

};

#endif //__PH_CONTROL_H_
