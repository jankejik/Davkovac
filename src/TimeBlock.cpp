#include "TimeBlock.h"
#include "MotorControl.h"

ChangeIcon TimeIcon(43, 44, 45);

time_pe blockBeginStart;
time_pe blockBeginEnd;

time_pe CircOnTime;
time_pe CircOffTime;

time_pe CircOnTime2;
time_pe CircOffTime2;

time_pe CircOnTime3;
time_pe CircOffTime3;

time_pe CircOnTime4;
time_pe CircOffTime4;

time_pe CircOnTime5;
time_pe CircOffTime5;

time_pe CircOnTime6;
time_pe CircOffTime6;

time_pe HeatOnTime;
time_pe HeatOffTime;

extern int32_pe pumpOffTime; // minut
Timer SteadyTimer;
bool auxRelOn = false;
bool TempRelOn = false;
bool vypisOhrevu = false;
extern Temp temp_01C;
int32_pe CircManual(0, 1);
int32_pe TempManual(0, 1);
int32_pe TempAuto(0, 1);
int32_pe nastTemperature(200, 400); // metry krychlove/10

void TimeInit(void)
{
    pinMode(RE_AUX_PIN, OUTPUT);
    pinMode(RE_TEP_PIN, OUTPUT);
    // pinMode(5, INPUT); // Fault vstup neni pouzit
    digitalWrite(RE_AUX_PIN, LOW);
    digitalWrite(RE_TEP_PIN, LOW);
    blockBeginStart.Init(0);
    blockBeginEnd.Init(0);

    CircOnTime.Init(0);
    CircOffTime.Init(0);

    CircOnTime2.Init(0);
    CircOffTime2.Init(0);

    CircOnTime3.Init(0);
    CircOffTime3.Init(0);

    CircOnTime4.Init(0);
    CircOffTime4.Init(0);
    CircManual.Init(0);

    CircOnTime5.Init(0);
    CircOffTime5.Init(0);

    CircOnTime6.Init(0);
    CircOffTime6.Init(0);

    HeatOnTime.Init(0);
    HeatOffTime.Init(0);
}

bool IsBlockTime(void)
{
    return TimeIcon.GetVal() == 2;
}
bool IsOnTime(time_pe &on, time_pe &off, int32_t actTime)
{
    bool retval = false;
    // on.Max(off.GetVal());
    // off.Min(on.GetVal());

    if (on.GetVal() < off.GetVal())
    {
        if (actTime > on.GetVal() && actTime < off.GetVal())
        {
            retval = true;
        }
    }

    return retval;
}
void TimeBlockTask(void)
{

    blockBeginStart.Max(blockBeginEnd.GetVal());
    blockBeginEnd.Min(blockBeginStart.GetVal());
    int32_t acttimesec = (hour()) * SECS_PER_HOUR + minute() * SEC_PER_MIN;
    if (IsOnTime(blockBeginStart, blockBeginEnd, acttimesec))
    {
        TimeIcon.SetVal(2);
    }
    else
    {
        if (blockBeginStart.GetVal() > blockBeginEnd.GetVal())
        {
            TimeIcon.SetVal(0);
        }
        else
        {
            TimeIcon.SetVal(1);
        }
    }

    auxRelOn = IsOnTime(CircOnTime, CircOffTime, acttimesec) || IsOnTime(CircOnTime2, CircOffTime2, acttimesec) || IsOnTime(CircOnTime3, CircOffTime3, acttimesec) || SteadyTimer.IsRunning() || IsOnTime(CircOnTime4, CircOffTime4, acttimesec) || IsOnTime(CircOnTime5, CircOffTime5, acttimesec) || IsOnTime(CircOnTime6, CircOffTime6, acttimesec);
    TempRelOn = IsOnTime(HeatOnTime, HeatOffTime, acttimesec);

    if (auxRelOn)
    {
        if (!digitalRead(RE_AUX_PIN))
        {
            SteadyTimer.StartMin(5);

            Msg("Cirkulace zapnuta");
            WebLog("Info", "Cirkulace zapnuta");
        }
        digitalWrite(RE_AUX_PIN, HIGH);
    }
    else
    {
        if (digitalRead(RE_AUX_PIN))
        {
            if (CircManual.GetVal() != 1)
            {
                Msg("Cirkulace vypnuta");
                WebLog("Info", "Cirkulace vypnuta");
            }
        }
        digitalWrite(RE_AUX_PIN, LOW);
    }

    if (CircManual.GetVal() == 1)
    {
        digitalWrite(RE_AUX_PIN, HIGH);
        Msg("Cirk zap man");
        WebLog("Info", "Cirkulace zapnuta manualne");
    }
    else if (CircManual.GetVal() == 0 && !auxRelOn)
    {
        digitalWrite(RE_AUX_PIN, LOW);
    }
    if (Motor::IsRelOn())
    {
        SteadyTimer.StartMin(pumpOffTime.GetVal());
    }
    if (TempAuto.GetVal() == 1)
    {
        if (TempRelOn && auxRelOn)
        {
            if (temp_01C.GetVal() < (nastTemperature - 10))
            {
                digitalWrite(RE_TEP_PIN, HIGH);
                if (!vypisOhrevu)
                {
                    vypisOhrevu = true;
                    Msg("Ohrev zapnuty");
                    WebLog("Info", "Ohrev zapnuty");
                }
            }
            else if (temp_01C.GetVal() > (nastTemperature + 10))
            {
                digitalWrite(RE_TEP_PIN, LOW);
                if (vypisOhrevu)
                {
                    vypisOhrevu = false;
                    Msg("Ohrev vypnuty");
                    WebLog("Info", "Ohrev vypnuty");
                }
            }
        }
        else
        {
            digitalWrite(RE_TEP_PIN, LOW);
            if (vypisOhrevu)
            {
                vypisOhrevu = false;
                Msg("Ohrev vypnuty");
                WebLog("Info", "Ohrev vypnuty");
            }
        }
    }
    else
    {
        if ((TempManual.GetVal() == 1) && auxRelOn)
        {
            digitalWrite(RE_TEP_PIN, HIGH);
            if (!vypisOhrevu)
            {
                vypisOhrevu = true;
                Msg("Ohrev zapnuty manualne");
                WebLog("Info", "Ohrev zapnuty manualne");
            }
        }
        else
        {
            digitalWrite(RE_TEP_PIN, LOW);
            if (vypisOhrevu)
            {
                vypisOhrevu = false;
                TempManual.SetVal(0);
                Msg("Ohrev vypnuty");
                WebLog("Info", "Ohrev vypnuty");
            }
        }
    }
}