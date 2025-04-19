#include "Buzzer.h"


Buzzer buzzer(BUZZER_PIN);

void Buzzer::Init(void)
{
    ledcSetup(CHANNEL_BUZZER, FREQ_BUZZER, RESOLUTION_BUZZER);
    ledcAttachPin(pin, CHANNEL_BUZZER);
}

void Buzzer::On(void)
{
    volum = BUZZER_VOL;
    state = true;
}
void Buzzer::On(int volume)
{
    if (volume > 250)
    {
        volume = 250;
        state = true;
    }
    else if (volume < 0)
    {
        volume = 0;
        state = false;
    }
    else
    {
        state = true;
    }
    volum = volume;
}
void Buzzer::On(int volume, int freq)
{
    ledcSetup(CHANNEL_BUZZER, freq, RESOLUTION_BUZZER);
    if (volume > 250)
    {
        volume = 250;
        state = true;
    }
    else if (volume < 0)
    {
        volume = 0;
        state = false;
    }
    else
    {
        state = true;
    }
    volum = volume;
}
void Buzzer::Off(void)
{
    state = false;
}
void Buzzer::StartOnTime(int time)
{
    currentTime = millis();
    setTime = time;
    onTime = true;
    volum = BUZZER_VOL;
}

void Buzzer::StartOnTime(int time, int volume)
{
    currentTime = millis();
    setTime = time;
    onTime = true;
    if (volume > 250)
    {
        volume = 250;
        state = true;
    }
    else if (volume < 0)
    {
        volume = 0;
        state = false;
    }
    else
    {
        state = true;
    }
    volum = volume;
}
void Buzzer::StartOnTime(int time, int volume, int freq)
{
    ledcSetup(CHANNEL_BUZZER, freq, RESOLUTION_BUZZER);
    currentTime = millis();
    setTime = time;
    onTime = true;
    if (volume > 250)
    {
        volume = 250;
        state = true;
    }
    else if (volume < 0)
    {
        volume = 0;
        state = false;
    }
    else
    {
        state = true;
    }
    volum = volume;
}
void Buzzer::Beep(int beepNumber, int beepTime)
{
    numberBeep = beepNumber * 2;
    beeTime = beepTime;
    onBeep = true;
    state = true;
    currentTime = millis();
    volum = BUZZER_VOL;
}
void Buzzer::Beep(int beepNumber, int beepTime, int volume)
{
    numberBeep = beepNumber * 2;
    beeTime = beepTime;
    onBeep = true;
    state = true;
    currentTime = millis();
    if (volume > 250)
    {
        volume = 250;
        state = true;
    }
    else if (volume < 0)
    {
        volume = 0;
        state = false;
    }
    else
    {
        state = true;
    }
    volum = volume;
}

void Buzzer::Beep(int beepNumber, int beepTime, int volume, int freq)
{
    ledcSetup(CHANNEL_BUZZER, freq, RESOLUTION_BUZZER);
    numberBeep = beepNumber * 2;
    beeTime = beepTime;
    onBeep = true;
    state = true;
    currentTime = millis();
    if (volume > 250)
    {
        volume = 250;
        state = true;
    }
    else if (volume < 0)
    {
        volume = 0;
        state = false;
    }
    else
    {
        state = true;
    }
    volum = volume;
}

void Buzzer::Update(void)
{
    if (onTime)
    {
        if ((currentTime + setTime) > millis())
        {
            state = true;
        }
        else
        {
            state = false;
            onTime = false;
        }
    }

    if (state)
    {
        ledcWrite(CHANNEL_BUZZER, volum);
    }
    else
    {
        ledcWrite(CHANNEL_BUZZER, BUZZER_OFF);
    }

    if (onBeep)
    {
        if (numberBeep > 0)
        {
            if ((currentTime + beeTime) < millis())
            {
                numberBeep--;
                if (numberBeep != 0)
                {
                    currentTime = millis();
                    state = !state;
                }
            }
        }
        else
        {
            onBeep = false;
            state = false;
        }
    }
    if (!onBeep && !state && !onTime)
    {
        ledcSetup(CHANNEL_BUZZER, FREQ_BUZZER, RESOLUTION_BUZZER);
    }
}

bool Buzzer::State(void)
{
    return state;
}
