/*! \file ORPControl.h
    \author Jan Kejík
*/

#include <Arduino.h>
#include "Param.h"
#include "ChangeIcon.h"
#include "Common.h"
#include "InfoMsg.h"

#ifndef __ORP_CONTROL_H_
#define __ORP_CONTROL_H_

#define DOSE_MAX_PER_DAY 10 // ml/m3 na den
#define DOSE_HISTORY_COUNT 50
#define ORP_CTRL_CALL_PERIOD 5000 // milisecond
#define ORP_CONTROL_ATTEMPS 10 


typedef struct 
{
    uint32_t time;
    uint32_t orp;
    uint32_t ontime;

}ORP_history_t;


class ORPControl
{
private:
    static uint32_t doseComulative;
    static uint32_t OnTime;
    static uint32_t timeDose[DOSE_HISTORY_COUNT];
    static float dose;
    static uint8_t Attempts;    // Pocet pokusu jednim cinidlem pro dosazeni stanovenoho chloru.

public:
    static void Init(void);
    static void Autocontrol(void);
    static bool SteadyTime(void);
     static void CalcCorection(int32_t deltamV);
     static void CtrlActionChlor(bool on, uint32_t time);


};

class ORPManualControl : public ChangeIcon 
{
    public:
    ORPManualControl(int32_t a, int32_t b): ChangeIcon(a,b){}
    bool SetVal(int32_t newVal);

    

};



#endif //__ORP_CONTROL_H_
