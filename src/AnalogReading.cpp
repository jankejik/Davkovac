#include "AnalogReading.h"

uint8_t AnalogReading::inputIndex = 0;
AnalogReading *AnalogReading::anaRead[4];
uint8_t AnalogReading::inputNumber = 0;
int32_t AnalogReading::actCount = 0;
int32_t AnalogReading::val[NUMBER_SAMPLES];

int32_t AnalogReading::AvgSample(int cnt)
{
    int32_t temp = 0;

   /* for (int i = 0; i < (cnt - 1); i++)
    {
        for (int j = i + 1; j < cnt; j++)
        {
            if (val[i] > val[j])
            {
                temp = val[i];
                val[i] = val[j];
                val[j] = temp;
            }
        }
    }
    temp = 0;*/
    for (int i = 0; i < (cnt); i++)
    {
        temp += val[i];
    }
    return temp / (cnt);
}

void AnalogReading::Reading_Task(void)
{
    AnalogReading *actAna = anaRead[inputIndex];

    if (actAna->enable)
    {
        val[actCount] = analogReadMilliVolts(actAna->anaPin);//actAna->readMiliVolts();
        actCount++;
        if (actCount >= actAna->avgCount)
        {
            int32_t temp = actAna->AvgSample(actCount);
            actAna->SetVal(temp);
            actCount = 0;
            inputIndex++;
            if (inputIndex >= inputNumber)
            {
                inputIndex = 0;
            }
        }
    }
    else
    {
        inputIndex++;
        if (inputIndex >= inputNumber)
        {
            inputIndex = 0;
        };
    }
}
void AnalogReading::Init(void)
{
    inputNumber = 0;
}