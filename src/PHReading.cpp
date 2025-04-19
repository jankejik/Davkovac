
#include "PHReading.h"
#include "InfoMsg.h"
#include "TempReading.h"
#include "TimeBlock.h"

extern AnalogReading VREFread;
AnalogReading PHRead;
extern Temp temp_01C;
int32_pe calphL_01(0, 140);
int32_pe calphH_01(0, 140);
int32_pe cal_mVL(-1500, 1500);
int32_pe cal_mVH(-1500, 1500);
int32_pe calphtemp_01(0, 500);
CalpH calpH;
PHReading pH_01;
// #define PH_DEBUG

void PHReading::Init(void)
{
    PHRead.Setup(PH_PIN_NUMBER, PH_AVG_CNT);
    CalCoef();
    calphL_01.Init(40);
    calphH_01.Init(100);
    cal_mVL.Init(193);
    cal_mVH.Init(-145);
    calphtemp_01.Init(25);
    lastCalTemp = -2000;
}

bool PHReading::IsCalValid(void)
{
    bool retval = calphH_01.IsValid() && calphL_01.IsValid() && cal_mVL.IsValid() && cal_mVH.IsValid();
    return retval;
}

bool PHReading::CalCoef(void)
{
    bool calOK = false;
    if (IsCalValid())
    {
        WebLog("pH", "Kalibrace pH dolní bod %.1f, %d mV", calphL_01.GetVal() / 10.0, cal_mVL.GetVal());
        WebLog("pH", "Kalibrace pH horní bod %.1f, %d mV", calphH_01.GetVal() / 10.0, cal_mVH.GetVal());

        if (calphH_01.GetVal() == 70 && calphL_01.GetVal() == 70)
        {
            LipH.SetCoef(1 / PH_COEF_DEF_25, PH_OFFSET_DEF - cal_mVL.GetVal() / PH_COEF_DEF_25);
        }
        else
        {
            LipH.SetCoef(cal_mVL.GetVal(), cal_mVH.GetVal(), calphL_01.GetVal(), calphH_01.GetVal());
        }
        calOK = LipH.IsValid();
    }

#ifdef PH_DEBUG

    dbSerialPrintln("PH");
    dbSerialPrintln(calphH_01.GetVal());
    dbSerialPrintln(calphL_01.GetVal());
    dbSerialPrintln(cal_mVH.GetVal());
    dbSerialPrintln(cal_mVL.GetVal());
#endif
    return calOK;
}

void PHReading::Update(void)
{
    if (auxRelOn)
    {

        int32_t valueV = GetmV();
        bool Ok = true;
        int32_t temp;
        if (LipH.IsValid())
        {
#ifdef TEMP_COMPENZATION
            int32_t aktTemp = temp_01C.GetVal();
            if (aktTemp != lastCalTemp)
            {
                lastCalTemp = aktTemp;
                int32_t deltaT = lastCalTemp - calphtemp_01.GetVal();
                LipH.TempCor(deltaT);
            }
#endif

            temp = LipH.Interpolate(valueV);
        }
        else
        {
            temp = PH_INVAL;
            Ok = false;
        }
        if (temp > PH_MAX)
        {
            temp = PH_MAX;
            Ok = false;
        }
        else if (temp < PH_MIN)
        {
            temp = PH_MIN;
            Ok = false;
        }
        if (!Ok)
        {
            if (!Error)
            {
                Err("Chyba pH sondy!");
                WebLog("ERROR", "Chyba pH sondy!");
                Error = true;
            }
        }
        else
        {
            Error = false;
        }
        filter.Input(temp);

        SetVal(filter.Output());
#ifdef PH_DEBUG
        dbSerialPrint("Update: ");
        dbSerialPrintln(valueV);
        dbSerialPrint("mV->pH:");
        dbSerialPrintln(GetmV());
        dbSerialPrint("mV offset:");
        dbSerialPrintln(VREFread.GetVal());
#endif
    }
}

int32_t PHReading::GetmV(void)
{
    return int32_t(((PHRead.GetVal() - VREFread.GetVal() * PH_K_OFFSET)) / PH_AMPLIFIER_GAIN);
}

bool CalpH::SetVal(int32_t newVal)
{
#ifdef PH_DEBUG
    dbSerialPrint("SetVal: ");
    dbSerialPrintln(newVal);
#endif

    if (newVal == -1)
    {
        calphtemp_01.SetVal(temp_01C.GetVal());
        int32_t temp = GetVal();
        if (temp > 70)
        {
            calphH_01.SetVal(temp);
            cal_mVH.SetVal(pH_01.GetmV());
            if (!pH_01.CalCoef())
            {
                calphL_01.SetVal(70);
                cal_mVL.SetVal(0);
            }
        }
        else
        {
            calphL_01.SetVal(temp);
            cal_mVL.SetVal(pH_01.GetmV());

            if (!pH_01.CalCoef())
            {
                calphH_01.SetVal(70);
                cal_mVH.SetVal(0);
            }
        }
        Msg("kal pH d. %.1f, %d mV", calphL_01.GetVal() / 10.0, cal_mVL.GetVal());

        WebLog("pH", "Kalibrace pH dolní bod %.1f, %d mV", calphL_01.GetVal() / 10.0, cal_mVL.GetVal());

        Msg("kal pH h. %.1f, %d mV", calphH_01.GetVal() / 10.0, cal_mVH.GetVal());

        WebLog("pH", "Kalibrace pH horní bod %.1f, %d mV", calphH_01.GetVal() / 10.0, cal_mVH.GetVal());
    }
    else
    {
        int32_p::SetVal(newVal);
    }
    return true;
}
