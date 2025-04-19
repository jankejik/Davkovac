#include "Flokulant.h"

Motor motor_floc(MOT4CTRL_PIN);

ChangeIcon FlocIcon(33, 30, 30 | BLINK);
FlocManualControl manualFlocCtrl(39, 4 | BLINK);

int32_pe flocAutoCtrl(0, 1);
int32_pe flocSetPoint(1, 100);

Timer Flokulant::periodTimer;
uint32_t Flokulant::OnTime = 0;
extern int32_pe poolVol;
extern Timer SteadyTimer;


void Flokulant::Init(void)
{
    flocAutoCtrl.Init(0);
    flocSetPoint.Init(30);
    periodTimer.StartMin(DELAY_DOSE_AFTER_POWER_ON_MIN);
}
void Flokulant::Autocontrol(void)
{
    if (flocAutoCtrl && !IsBlockTime() && auxRelOn && !SteadyTimer.IsRunning())
    {
        if (periodTimer.IsElapsed())
        {
            periodTimer.StartMin(MIN_PER_HOUR*HOUR_PER_DAY);
            uint32_t dose = UNIT_POOL_VOL_M3 * poolVol.GetVal() * flocSetPoint.GetVal();
            OnTime = dose * SEC_PER_MIN;
            OnTime /= pumpCapacityFl;
            Msg("Vlockovac %dml (%ds)", dose, OnTime);
            WebLog("Info", "davka vlockovace %dml (%ds)", dose, OnTime);
            motor_floc.Set(OnTime);
        }
    }

    if (motor_floc.IsOn())
    {
        FlocIcon = 2;
    }
    else if (flocAutoCtrl.GetVal())
    {
        FlocIcon = 1;
    }
    else
    {
        FlocIcon = 0;
    }
}

bool FlocManualControl::SetVal(int32_t newVal)
{
    if (!flocAutoCtrl.GetVal())
    {
        if (newVal == 1)
        {

            dbSerialPrintln("Manual Chlor");
            motor_floc.Set(10);
        }
        else
        {
            motor_floc.Set(0);
        }
        ChangeIcon::SetVal(newVal);
    }

    return true;
}