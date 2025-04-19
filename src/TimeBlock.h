
/*! \file TimeBlock.h
    \author Jan Kejík
*/

#ifndef __TIME_BLOCK_H_
#define __TIME_BLOCK_H_

#include "Arduino.h"
#include "Common.h"
#include "Param.h"
#include "Web.h"
#include "ChangeIcon.h"
#include "InfoMsg.h"
#include "TempReading.h"
#define RE_AUX_PIN 14
#define RE_TEP_PIN 47

#define TIME_BLOCK_PERIOD 3000
extern ChangeIcon TimeIcon;

extern bool auxRelOn;


extern time_pe blockBeginStart;
extern time_pe blockBeginEnd;

extern time_pe CircOnTime;
extern time_pe CircOffTime;

extern time_pe CircOnTime2;
extern time_pe CircOffTime2;

extern time_pe CircOnTime3;
extern time_pe CircOffTime3;

extern time_pe CircOnTime4;
extern time_pe CircOffTime4;

extern time_pe CircOnTime5;
extern time_pe CircOffTime5;

extern time_pe CircOnTime6;
extern time_pe CircOffTime6;

extern time_pe HeatOnTime;
extern time_pe HeatOffTime;

extern int32_pe CircManual;
extern int32_pe nastTemperature;

void TimeBlockTask(void);
void TimeInit(void);

bool IsBlockTime(void);

class Timer
{
private:
    uint32_t startTime;
    uint32_t deltaT;
    bool run;
    bool elapsed;

public:
    Timer(void)
    {
        run = false;
        elapsed = false;
    }
    void StartMs(uint32_t delT)
    {
        if (delT > 0)
        {
            deltaT = delT;
            run = true;
            startTime = millis();
            elapsed = false;
        }
        else
        {
            run =  false;
            elapsed = false;
        }
    }
    void StartSec(uint32_t delT)
    {
        StartMs(delT*1000);
    }
    void StartMin(uint32_t delT)
    {
        StartSec(delT*60);
    }
    bool IsDeltaTime(void)
    {
        return deltaT > 0;
    }
    bool IsElapsed(void)
    {

        if (!elapsed && run)
        {
            uint32_t tempT = millis() - startTime;
            if (tempT > deltaT)
            {
                elapsed = true;
                run = false;
            }
        }
        return elapsed;
    }
    void Stop(void)
    {
        run = false;
        elapsed = false;
    }

    bool IsRunning(void)
    {
        if (run)
        {
            uint32_t tempT = millis() - startTime;
            if (tempT > deltaT)
            {
                elapsed = true;
                run = false;
            }
        }
        return run;
    }
};

#endif /*__TIME_BLOCK_H_*/