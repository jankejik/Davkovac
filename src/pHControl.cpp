#include "pHControl.h"
#include "InfoMsg.h"

int32_pe pumpOffTime(1, 200); // minut
int32_pe pHDownDose(1, 150);  // mililitru/m3 na 1pH
int32_pe pHUPDose(1, 150);    // mililitru/m3 na 1pH

int32_pe poolVol(1, 2000); // metry krychlove/10

Motor motor_pHplus(MOT2CTRL_PIN);
Motor motor_pHminus(MOT1CTRL_PIN);

ChangeIcon pHplusIcon(32, 30, 30 | BLINK);
ChangeIcon pHminusIcon(32, 28, 28 | BLINK);
pHManualControl manualpHCtrl(2, 3 | BLINK, 4 | BLINK);

int32_pe phSetPoint(0, 140);
int32_pe phAutoCtrlD(0, 1);
int32_pe phAutoCtrlU(0, 1);

extern PHReading pH_01;

uint8_t pHControl::altAttempts = 0;
uint8_t pHControl::Attempts = 0;
Control_t pHControl::lastCtrl = Ctrl_done;
uint32_t pHControl::dose = 0;
uint32_t pHControl::OnTime = 0;

extern Timer SteadyTimer;
void pHControl::Init(void)
{
    pumpOffTime.Init(30);
    pHDownDose.Init(30);
    pHUPDose.Init(30);
    phSetPoint.Init(70);
    phAutoCtrlD.Init(0);
    phAutoCtrlU.Init(0);
    poolVol.Init(10);
    SteadyTimer.StartSec(300);
    Attempts = 0;
}

void pHControl::CalcCorection(int32_t deltapH)
{

    if (deltapH > 0)
    {
        dose = UNIT_POOL_VOL_M3 * poolVol.GetVal() * pHUPDose.GetVal() * deltapH * UNIT_PH;
        OnTime = dose * SEC_PER_MIN;
        OnTime /= pumpCapacitypHplus;
    }
    else if (deltapH < 0)
    {
        dose = -UNIT_POOL_VOL_M3 * poolVol.GetVal() * pHDownDose.GetVal() * deltapH * UNIT_PH;
        OnTime = dose * SEC_PER_MIN;
        OnTime /= pumpCapacitypHminus;
    }
}

bool pHControl::CtrlError(void)
{
    static bool error = false;

    if (!error)
    {
        if (Attempts > MAX_CTRL_ATTEMPTS)
        {
            error = true;
            Err("Nelze dosahnout pH!");
            WebLog("ERROR", "Nelze dosahnout pH!");
        }
        else

            if (altAttempts > MAX_CTRL_ATTEMPTS)
        {
            Err("Chyba davkovani pH!");
            WebLog("ERROR", "Chyba davkovani pH!");
            error = true;
        }
    }
    else if ((Attempts < MAX_CTRL_ATTEMPTS) && (altAttempts < MAX_CTRL_ATTEMPTS))
    {
        error = false;
    }

    return error;
}

void pHControl::pHAutocontrol(void)
{
    int32_t dif = phSetPoint.GetVal() - pH_01.GetVal();
    uint32_t up = phAutoCtrlU.GetVal();
    uint32_t down = phAutoCtrlD.GetVal();

    if (dif >= PH_CONTROL_DEATH_ZONE)
    {
        // Skutecna pH je nizka je potreba pH plus cinidlo.
        if (!CtrlError())
        {
            if (up)
            {

                lastCtrl = Ctrl_up;
                CalcCorection(dif);
                CtrlActionUp(true, OnTime);
                motor_pHminus.Set(0);
            }
        }
        else if (lastCtrl == Ctrl_up && !up)
        {
            if (Attempts > 0)
            {
                WebLog("Info", "pH+ bylo doregulováno po %d dávkách", Attempts);
            }
            Attempts = 0;
            altAttempts = 0;
        }
        else
        {
            motor_pHminus.Set(0);
        }
    }
    else if (dif <= -PH_CONTROL_DEATH_ZONE)
    {
        if (!CtrlError())
        {

            if (down)
            {
                lastCtrl = Ctrl_down;
                CalcCorection(dif);
                CtrlActionDown(true, OnTime);
                motor_pHplus.Set(0);
            }
        }
        else if (lastCtrl == Ctrl_down && !down)
        {
            if (Attempts > 0)
            {
                WebLog("Info", "pH- bylo doregulováno po %d dávkách", Attempts);
            }
            Attempts = 0;
            altAttempts = 0;
        }
        else
        {
            motor_pHplus.Set(0);
        }
    }
    else
    {
        if (up)
        {
            motor_pHplus.Set(0);
        }
        if (down)
        {
            motor_pHminus.Set(0);
        }

        lastCtrl = Ctrl_done;
        altAttempts = 0;
        Attempts = 0;
    }

    if (motor_pHplus.IsOn())
    {
        pHplusIcon = 2;
    }
    else if (up)
    {
        pHplusIcon = 1;
    }
    else
    {
        pHplusIcon = 0;
    }

    if (motor_pHminus.IsOn())
    {
        pHminusIcon = 2;
    }
    else if (down)
    {
        pHminusIcon = 1;
    }
    else
    {
        pHminusIcon = 0;
    }
}

void pHControl::CtrlActionUp(bool on, uint32_t time)
{
    if (on && !SteadyTime() && auxRelOn)
    {
        motor_pHplus.Set(time);
        SteadyTimer.StartMin(pumpOffTime.GetVal());

        if (lastCtrl == Ctrl_down)
        {
            altAttempts++;
        }
        else if (lastCtrl == Ctrl_up)
        {
            Attempts++;
        }

        Msg("pH+ davka %dml (%ds)", dose, time);
        WebLog("Info", "pH+ davka %dml (%ds)", dose, time);
        // WebLog("Info: ", "%d. davka", Attempts);
    }
    else if (!on && motor_pHplus.IsOn())
    {
        motor_pHplus.Set(0);
    }
}

void pHControl::CtrlActionDown(bool on, uint32_t time)
{
    if (on && !SteadyTime() && auxRelOn)
    {
        motor_pHminus.Set(time);
        SteadyTimer.StartMin(pumpOffTime.GetVal());

        if (lastCtrl == Ctrl_down)
        {
            Attempts++;
        }
        else if (lastCtrl == Ctrl_up)
        {
            altAttempts++;
        }

        Msg("pH- davka %dml (%ds)", dose, time);
        WebLog("Info", "pH- davka %dml (%ds)", dose, time);
    }
    else if (!on && motor_pHminus.IsOn())
    {
        motor_pHminus.Set(0);
    }
}

bool pHControl::SteadyTime(void)
{
    return (SteadyTimer.IsRunning() || IsBlockTime());
}

bool pHManualControl::SetVal(int32_t newVal)
{
    if (!phAutoCtrlD.GetVal())
    {
        if (newVal == 1)
        {
            dbSerialPrintln("Manual Down");
            motor_pHminus.Set(10);
        }
        else
        {
            motor_pHminus.Set(0);
        }
        ChangeIcon::SetVal(newVal);
    }

    if (!phAutoCtrlU.GetVal())
    {
        if (newVal == 2)
        {
            dbSerialPrintln("Manual UP");
            motor_pHplus.Set(10);
        }
        else
        {
            motor_pHplus.Set(0);
        }
        ChangeIcon::SetVal(newVal);
    }

    return true;
}
