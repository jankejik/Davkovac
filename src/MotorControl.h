/*! \file MotorControl.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "Common.h"
#include "Param.h"
#include "TimeBlock.h"

#ifndef __MOTOR_CONTROL_H_
#define __MOTOR_CONTROL_H_

#define REPUMP_PIN 21

#define MOT1CTRL_PIN 38
#define MOT2CTRL_PIN 39
#define MOT3CTRL_PIN 40
#define MOT4CTRL_PIN 41

#define INHIB_ON_DELAY 100
#define RELE_OFF_DELAY 2000
#define MOTOR_CTRL_PERIOD 5000
#define MOTOR_UPDATE_PERIOD 10
#define MAX_ON_TIME 600

extern int32_pe pumpCapacityDez;
extern int32_pe pumpCapacitypHplus;
extern int32_pe pumpCapacitypHminus;
extern int32_pe pumpCapacityFl;


class Motor : public param
{
private:
    static Timer idleTimer;
    static Timer relTime;
    static Motor *motorInst[4];
    static uint8_t motorCount;
    Timer runTime;
    uint8_t motPin;
    bool test;

public:
    Motor(uint8_t pin)
    {
        motPin = pin;
        if (motorCount < 4)
        {
            motorInst[motorCount] = this;
            motorCount++;
        }
    }
    static void CtrlTask(void);
    void Set(uint32_t tim);
    void Stop(void);
    bool IsOn(void);
    static void StopAll(void);
    void Update(void);
    void Init(void);

    static void InitAll(void);
    static bool IsRelOn(void);
   
    bool SetVal(int32_t newVal);
    
    int32_t GetVal(void) ;   
    void GetString(String &retVal);
};

#endif //__MOTOR_CONTROL_H_