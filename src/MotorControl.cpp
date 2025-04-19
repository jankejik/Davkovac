#include "MotorControl.h"
#include "InfoMsg.h"

Timer Motor::idleTimer;
Timer Motor::relTime;
Motor *Motor::motorInst[];
uint8_t Motor::motorCount = 0;
int32_p motorTestTime(1, 1, 200);

int32_pe pumpCapacityDez(1, 200);
int32_pe pumpCapacitypHplus(1, 200);
int32_pe pumpCapacitypHminus(1, 200);
int32_pe pumpCapacityFl(1, 200);

void Motor::Init(void)
{
    pinMode(motPin, OUTPUT);
    digitalWrite(motPin, LOW);
    test = false;
}
void Motor::InitAll(void)
{
    pinMode(REPUMP_PIN, OUTPUT);
    digitalWrite(REPUMP_PIN, LOW);
    pumpCapacityDez.Init(39);
    pumpCapacitypHplus.Init(39);
    pumpCapacitypHminus.Init(39);
    pumpCapacityFl.Init(39);
     for (uint8_t i = 0; i < motorCount; i++)
    {
        motorInst[i]->Init();
    }
}

bool Motor::IsOn(void)
{
    return digitalRead(motPin) == HIGH;
}
bool Motor::IsRelOn(void)
{
    return digitalRead(REPUMP_PIN) == HIGH;
}
void Motor::Update(void)
{
    if (IsRelOn())
    {
        if (IsOn())
        {

            if (runTime.IsRunning())
            {
                relTime.StartMs(RELE_OFF_DELAY);
            }
            else
            {
                Stop();
            }
        }
        else if (runTime.IsRunning() && !idleTimer.IsRunning())
        {

            digitalWrite(motPin, HIGH);
            dbSerialPrintln("Mot ON");
            change = true;
            idleTimer.StartMs(INHIB_ON_DELAY);
        }
    }
    else if (runTime.IsRunning())
    {
        digitalWrite(REPUMP_PIN, HIGH);
        dbSerialPrintln("Rele ON");
        idleTimer.StartMs(INHIB_ON_DELAY);
        relTime.StartMs(RELE_OFF_DELAY);
    }
}
void Motor::CtrlTask(void)
{
    for (uint8_t i = 0; i < motorCount; i++)
    {
        motorInst[i]->Update();
    }
    if (relTime.IsElapsed())
    {
        relTime.Stop();
        digitalWrite(REPUMP_PIN, LOW);
        dbSerialPrintln("Rele off");
    }
}
void Motor::Set(uint32_t tim)
{
    if (!test)
    {
        if (tim > MAX_ON_TIME)
        {
            tim = MAX_ON_TIME;
            Msg("Omezena doba cerpani na 600s");
            WebLog("Varovani", "Byla omezena doba čerpání na maximální hodnotu (600s)");
        }
        runTime.StartSec(tim);
    }
}

void Motor::StopAll(void)
{
    digitalWrite(REPUMP_PIN, LOW);
    for (uint8_t i = 0; i < motorCount; i++)
    {
        motorInst[i]->Stop();
    }
}

bool Motor::SetVal(int32_t newVal)
{
    if (newVal == 1)
    {
        if (IsOn())
        {
            Stop();
        }
        else
        {
            Set(motorTestTime.GetVal());
            test = true;
        }
    }
    return true;
}
int32_t Motor::GetVal(void)
{
    return IsOn() ? 0 : 1;
}

void Motor::GetString(String &retVal)
{
    retVal = (int32_t)IsOn();
    change = false;
}

void Motor::Stop(void)
{
    digitalWrite(motPin, LOW);
    dbSerialPrintln("Mot OFF");
    runTime.Stop();
    test = false;
    change = true;
}