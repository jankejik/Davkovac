
#include "ORPReading.h"
#include "InfoMsg.h"
#include "TimeBlock.h"

AnalogReading ORPRead;
ORPreading orp;
extern AnalogReading VREFread;

int32_t ORPreading::GetVal(void)
{

    return value;
}
void ORPreading::Init(void)
{
    ORPRead.Setup(ORP_ANALOG_INPUT, ORP_AVG_COUNT);
}
void ORPreading::Update(void)
{
    if (auxRelOn)
    {
        bool orpOK = true;

        int32_t temp = ORPRead.GetVal();
        ORPprint("ORP out: ");
        ORPprint(temp);

        temp = int32_t((float(temp) - float(VREFread.GetVal()) * ORP_K_COEF) / ORP_GAIN);
        ORPprint(" ORP inp: ");
        ORPprint(temp);
        ORPmv.Input(temp);
        if (temp > ORP_VAL_MAX)
        {
            orpOK = false;
            temp = ORP_VAL_MAX;
            ORPmv.Init(ORP_VAL_MAX);
        }
        else if (temp < ORP_VAL_MIN)
        {
            orpOK = false;
            temp = ORP_VAL_MIN;
            ORPmv.Init(ORP_VAL_MIN);
        }
        if (!orpOK)
        {
            if (!Error)
            {
                Err("Chyba ORP sondy");
                WebLog("ERROR", "Chyba ORP sondy!");
                Error = true;
            }
        }
        else
        {
            Error = false;
        }
        temp = ORPmv.Output();
        SetVal(temp);
    }
}
