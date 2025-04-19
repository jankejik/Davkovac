/*! \file TempReading.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "Param.h"
#include "AnalogReading.h"
#include "ChangeIcon.h"
#include "Common.h"

#ifndef __TEMP_REAGING_H_
#define __TEMP_REAGING_H_

#define VREF_PIN_NUMBER 4
#define NTC_PIN_NUMBER 7 // Analogový pin, ke kterému je termistor připojen
#define NTC_AVG_CNT 200
#define VREF_AVG_CNT 200

#define NTC_RESIS_REF 10000 // Referenční odpor termistoru
#define NTC_BETA_COEF 3950
#define NTC_TEMP_REF 25 // Teplota pro referenční odpor
#define NTC_PULL_UP_RESIS 4700
#define TEMP_READING_PERIOD 4000
#define ADC 4095

#define INVALID_TEMPERATURE -999

extern ChangeIcon tempIcon;

class Temp : public param
{
private:
    int32_t avgVal;
    float vref_mv;
    float vntc_mv;
    bool refReading;
    int32_t AvgCount;
    float ntc;
    bool senc_ok;
    ExpFilter temperature;

public:
    Temp(void) {}
    int32_t GetVal(void);
    void Temp_Reading(void);
    void GetString(String &retVal);
    void Init(void);
    static bool sencError;
};

#endif /* __TEMP_REAGING_H_ */
