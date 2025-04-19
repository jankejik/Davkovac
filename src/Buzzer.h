#include "Arduino.h"

#define FREQ_BUZZER 1000
#define CHANNEL_BUZZER 1
#define RESOLUTION_BUZZER 8
#define BUZZER_PIN 3
#define BUZZER_VOL 255
#define BUZZER_OFF 0

#define BUZZER_READING 10

class Buzzer
{
private:
    int pin;
    int frequency;
    int currentTime;
    int setTime;
    bool state;
    int volum;
    bool onTime;
    int numberBeep;
    bool onBeep;
    int beeTime;

public:
    Buzzer(int pinNumber, int freq = 2000)
    {
        pin = pinNumber;
        frequency = freq;
        state = false;
        volum = BUZZER_VOL;
    }
    void Init(void);
    bool State(void);

    void On(void);
    void On(int volume);
    void On(int volume, int freq);

    void Off(void);

    void StartOnTime(int time);
    void StartOnTime(int time, int volume);
    void StartOnTime(int time, int volume, int freq);

    void Beep(int beepNumber, int beepTime);
    void Beep(int beepNumber, int beepTime, int volume);
    void Beep(int beepNumber, int beepTime, int volume, int freq);

    void Update(void);
};


extern Buzzer buzzer;