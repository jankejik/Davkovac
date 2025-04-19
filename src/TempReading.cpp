#include "TempReading.h"
#include "InfoMsg.h"

#define ADC_MAX_VAL 4095
#define ADC_MAX_VOLT 3300

bool Temp::sencError = true;

AnalogReading VREFread;
AnalogReading TempRead;
//#define TEMP_DEBUG

void Temp::Init(void)
{
    TempRead.Setup(NTC_PIN_NUMBER, NTC_AVG_CNT);
    VREFread.Setup(VREF_PIN_NUMBER, VREF_AVG_CNT);
    temperature.Init();

    refReading = true;
    AvgCount = 0;
}

void Temp::Temp_Reading(void)
{
    float temp = 0;
    int32_t tempint = VREFread.GetVal();
    
    VREFread.ReadingEnable(true);
    vref_mv = tempint;

#ifdef TEMP_DEBUG
    dbSerialPrint("Vref:");
    dbSerialPrintln(vref_mv);
#endif

    vntc_mv = TempRead.GetVal();
    // Serial.println(TempRead.GetVal());
#ifdef TEMP_DEBUG
dbSerialPrint("Vtemp:");
    dbSerialPrintln(vntc_mv);
#endif
    senc_ok = vntc_mv < (vref_mv - 10);

    if (senc_ok)
    {
        ntc = vntc_mv * NTC_PULL_UP_RESIS / (vref_mv - vntc_mv);

        temp = ntc / NTC_RESIS_REF;            // (R/Ro)
        temp = log(temp);                      // ln(R/Ro)
        temp /= NTC_BETA_COEF;                 // 1/B * ln(R/Ro)
        temp += 1.0 / (NTC_TEMP_REF + 273.15); // + (1/To)
        temp = 1.0 / temp;                     // Převrácená hodnota
        temp -= 273.15;                        // Převod z Kelvinů na stupně Celsi
        sencError = false;
#ifdef TEMP_DEBUG
        dbSerialPrintln(temp);
#endif
        temperature.InputF(temp * 10);

        change = true;
#ifdef TEMP_DEBUG
        dbSerialPrint("Spoctena teplota:");
        dbSerialPrintln(temperature.Output());
#endif
    }
    else
    {
        if (!sencError)
        {
            Err("Chyba cidla teploty!");
            WebLog("ERROR", "Chyba čidla teploty!");
            sencError = true;
        }
        change = true;
        temperature.Init();
    }
}

int32_t Temp::GetVal(void)
{
    return temperature.Output();
}

void Temp::GetString(String &retVal)
{

    retVal = temperature.Output();
    change = false;
}