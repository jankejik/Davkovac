/*! \file Flokulant.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "MotorControl.h"
#include "ChangeIcon.h"
#include "Param.h"
#include "pHControl.h"
#include "Common.h"
#include "TimeBlock.h"
#include "InfoMsg.h"


#ifndef __FLOKULANT_H_
#define __FLOKULANT_H_

#define FLOC_CTRL_CALL_PERIOD 3500 // milisecond
#define DELAY_DOSE_AFTER_POWER_ON_MIN 10 // minut

class Flokulant
{
private:
    static Timer periodTimer;
    static uint32_t OnTime;

public:
    static void Init(void);
    static void Autocontrol(void);
};

class FlocManualControl : public ChangeIcon
{
public:
    FlocManualControl(int32_t a, int32_t b) : ChangeIcon(a, b) {}
    bool SetVal(int32_t newVal);
};

#endif //__FLOKULANT_H_