
/*! \file AnalogReading.h
    \author Jan Kejík
*/

#ifndef __ANALOG_REAGING_H_
#define __ANALOG_REAGING_H_

#include "Arduino.h"
#include "Param.h"
#include "Common.h"
#include "InfoMsg.h"

#define ANALOG_READING_PERIOD 1
#define INVALID_READING -1
#define NUMBER_SAMPLES 200
#define TEMP_COEF (0.001984f) // Teplotni koeficient smernice primky [mV/(0.1pH*0.1°C)]

#ifdef ANA_DEBUG

#define ANAprint(a) dbSerial.print(a)
#define ANAprintln(a) dbSerial.println(a)

#else

#define ANAprint(a)
#define ANAprintln(a)

#endif

class AnalogReading : public param
{
private:
    int32_t value_mV;
    int32_t avgCount;
    static uint8_t inputIndex;
    static AnalogReading *anaRead[4];
    static uint8_t inputNumber;
    static int32_t actCount;
    static int32_t val[NUMBER_SAMPLES];
    int8_t anaPin;
    bool enable;

public:
    AnalogReading(void)
    {
        value_mV = INVALID_READING;
        actCount = 0;
        enable = false;
    }
    static void Reading_Task(void);
    static void Init(void);
    int32_t AvgSample(int cnt);

    void ReadingEnable(bool en)
    {
        enable = en;
    }
    void Setup(int pin, uint8_t numberAvg = NUMBER_SAMPLES)
    {
        adcAttachPin(pin);
        anaPin = pin;
        if (inputNumber < 4)
        {
            anaRead[inputNumber] = this;
            inputNumber++;
            enable = true;
        }
        avgCount = numberAvg;
    }

    void GetString(String &retVal)
    {
        retVal = GetVal();
        change = false;
    }
    int32_t GetVal(void)
    {
        return value_mV;
    }
    bool SetVal(int32_t newVal)
    {

        if (newVal != value_mV)
        {
            change = true;
            value_mV = newVal;
        }
        return true;
    }
};

class ExpFilter
{
private:
    float val;
    const int32_t coef;
    bool valid;

public:
    ExpFilter(int32_t cf) : coef(cf)
    {
        val = 0;
        valid = false;
    }
    ExpFilter(void) : coef(2)
    {
        val = 0;
        valid = false;
    }

    int32_t Input(int32_t input)
    {
        if (valid)
        {
            val = (float)input;
            valid = true;
        }
        else
        {
            val = val + ((float)input - val) / (float)coef;
        }
        return val;
    }
    int32_t InputF(float input)
    {
        int32_t temp = input + 0.5;
        return Input(temp);
    }
    void Init(int32_t input = 0)
    {
        val = (float)input;
        valid = false;
    }
    bool isValid(void)
    {
        return valid;
    }
    int32_t Output(void)
    {
        return (int32_t)val;
    }
};

class LinInterpol
{
private:
    float coef;
    float offset;
    float corCoef;

public:
    void SetCoef(int32_t mVL, int32_t mVH, int32_t valL, int32_t valH)
    {
        corCoef = 0;
        if (mVL != mVH)
        {
            float deltay = valH - valL;
            float deltax = mVH - mVL;
            coef = deltay / deltax;
            offset = valH - coef * mVH;
            corCoef = deltay / deltax;
        }
        else
        {
            coef = NAN;
            offset = NAN;
        }
        dbSerialPrint("Coef: ");
        dbSerialPrint(coef);
        dbSerialPrint(", offset: ");
        dbSerialPrintln(offset);
    }

    void SetCoef(float c, float o)
    {
        coef = c;
        offset = o;
        corCoef = c;
        dbSerialPrint("Def Coef: ");
        dbSerialPrint(coef);
        dbSerialPrint(", offset: ");
        dbSerialPrintln(offset);
    }
    void TempCor(int32_t deltaT)
    {
        float cor = deltaT * TEMP_COEF;
        corCoef = coef;
        float c = 1 / corCoef;
        if (cor != 0)
        {
            c -= cor;
            dbSerialPrint("Cor: ");
            dbSerialPrint(deltaT);
            dbSerialPrint(" coef: ");
            dbSerialPrint(c);
            corCoef = 1 / c;
        }
        //Msg("pH kor %dC, %f", deltaT / 10, c);
    }
    int32_t Interpolate(int32_t mV)
    {
        if (IsValid())
        {
            //Msg("pH= %f x %fmV + %f", corCoef, mV, offset);
            return (int32_t)((corCoef) * (float)mV + offset);
        }
        return 0;
    }
    bool IsValid(void)
    {
        return !isnan(coef) && !isnan(offset);
    }
};

#endif /*__ANALOG_REAGING_H_ */
