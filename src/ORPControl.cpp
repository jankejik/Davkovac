#include "ORPControl.h"
#include "MotorControl.h"
#include "pHControl.h"
#include "ORPReading.h"
#include "TimeBlock.h"

Motor motor_chlor(MOT3CTRL_PIN);

ChangeIcon OrpIcon(31, 30, 30 | BLINK);

ORPManualControl manualORPCtrl(1, 4 | BLINK);
extern int32_pe pumpOffTime;
extern ORPreading orp;

int32_pe orpAutoCtrl(0, 1);
int32_pe orpSetPoint(100, 1000);
int32_pe orpDose(1, 500); // mililitru/m3
extern int32_pe poolVol;

float ORPControl::dose = 0;
uint32_t ORPControl::OnTime = 0;
uint8_t ORPControl::Attempts = 0;

extern Timer SteadyTimer;

void ORPControl::Init(void)
{
    orpAutoCtrl.Init(0);
    orpSetPoint.Init(500);
    orpDose.Init(30);
    Attempts = 0;
}

void ORPControl::CalcCorection(int32_t deltamV)
{

    OnTime = 0;
    if (deltamV > 0)
    {
        dose = orpDose.GetVal();
        OnTime = dose * SEC_PER_MIN;
        OnTime /= pumpCapacityDez;
    }
}

void ORPControl::Autocontrol(void)
{

    if (orpAutoCtrl.GetVal())
    {

        int32_t dif = orpSetPoint.GetVal() - orp.GetVal();

        if (dif > 0)
        {
            if (Attempts < ORP_CONTROL_ATTEMPS)
            {

                CalcCorection(dif);
                CtrlActionChlor(true, OnTime);
            }
            else if (Attempts == ORP_CONTROL_ATTEMPS)
            {

                WebLog("Varování", "Nepodařilo se dosáhnout požadované hodnoty ORP!");
                Attempts++;
            }
        }
        else
        {
            if (Attempts > 0)
            {
                WebLog("Info", "ORP bylo doregulováno po %d dávkách", Attempts);
            }
            CtrlActionChlor(false, 0);
            Attempts = 0;
        }
    }
    else
    {
        Attempts = 0;
    }

    if (motor_chlor.IsOn())
    {
        OrpIcon = 2;
    }
    else if (orpAutoCtrl.GetVal())
    {
        OrpIcon = 1;
    }
    else
    {
        OrpIcon = 0;
    }
}

void ORPControl::CtrlActionChlor(bool on, uint32_t time)
{

    if (on && !SteadyTime() && auxRelOn)
    {
        motor_chlor.Set(time);
        SteadyTimer.StartMin(pumpOffTime.GetVal());
        Attempts++;
        Msg("Chloru %.0fml (%ds)", dose, time);
        WebLog("Info", "Dávka chlóru %.0fml (%ds)", dose, time);
    }
    else if (!on && motor_chlor.IsOn())
    {
        motor_chlor.Set(0);
    }
}

bool ORPControl::SteadyTime(void)
{
    return (SteadyTimer.IsRunning() || IsBlockTime());
}

bool ORPManualControl::SetVal(int32_t newVal)
{
    if (!orpAutoCtrl.GetVal())
    {

        if (newVal == 1)
        {

            dbSerialPrintln("Manual Chlor");
            motor_chlor.Set(10);
            ChangeIcon::SetVal(newVal);
        }
        else
        {
            motor_chlor.Set(0);
            ChangeIcon::SetVal(newVal);
        }
    }

    return true;
}
