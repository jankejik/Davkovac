/*! \file PHReading.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "Param.h"
#include "AnalogReading.h"
#include "Common.h"

#ifndef __PH_REAGING_H_
#define __PH_REAGING_H_

#define PH_PIN_NUMBER 5
#define PH_AVG_CNT 200

// Kalibrovana hodnota pH dolní bod 193 mV
// Kalibrovana hodnota pH horní bod -145 mV

#define PH_COEF_DEF_0 (54.20f) // mV/pH pH

#define PH_COEF_DEF_25 (59.16f)  // mV/pH
#define PH_COEF_DEF_100 (74.04f) // mV/pH

#define PH_OFFSET_DEF (70.0f) // pH pro 0V

#define PH_READING_PERIOD 4000
#define PH_INVAL 0
#define PH_AMPLIFIER_GAIN -1.363636364
#define PH_K_OFFSET 0.436758893

#define PH_MAX 140
#define PH_MIN 0

//#define TEMP_COMPENZATION

/*
    vystupni napeti prevodniku pH je:
    pHout = Gph * (Vph-Vref)
    Vref zhruba 1V (odvozeno z reference 2.5V).
    Gph je zesileni zesilovace a je PH_AMPLIFIER_GAIN.
    Vysledne pH je korigovano linearni interpolaci dvou kalibracnich bodu.
*/
class PHReading : public int32_p
{
private:
    LinInterpol LipH;
    LinInterpol LiTep;
    bool Error;
    int32_t lastCalTemp;
    ExpFilter filter;

public:
    PHReading(void) : filter(8) {}
    // Konverze hodnoty AD prevodu napeti sondy a referencniho napeti na vysledne pH
    void Update(void);
    void Init(void);
    // Vraci stav kalibracnich konstant ulozenych v EEPROM tj. jestli hodnoty v EEprom jsou platne
    bool IsCalValid(void);
    // Prevede definici konverzni primky dvojici bodu [pHmin, Vmin] a [pHmax, Vmax]
    // na smernici [coef] a offset primky [offset]
    bool CalCoef(void);
    // Vraci vypocitane napeti na vstupu sondy
    int32_t GetmV(void);
};

/*
    Trida pouzivana pro kalibraci pH sondy, udrzuje pozadovanou
    hodnotu aktualniho pH a na zapis prikazu (hodnoty -1) ulozi
    tuto hodnotu spolu s aktualnim napetim sondy do jednoho ze
    dvou kalibracnich bodu.

*/
class CalpH : public int32_p
{

public:
    CalpH(void) : int32_p(70, 0, 140){};
    bool SetVal(int32_t newVal);
};

#endif /* __PH_REAGING_H_ */