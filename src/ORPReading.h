/*! \file ORPReading.h
    \author Jan Kejík
*/


#include "Arduino.h"
#include "Param.h"
#include "AnalogReading.h"
#include "Common.h"

#ifndef __ORP_REAGING_H_
#define __ORP_REAGING_H_

#define ORP_UPDATE_PERIOD 4000
#define ORP_ANALOG_INPUT 6
#define ORP_AVG_COUNT 200
#define ORP_GAIN (-0.5f)
#define FLASH_UPDATE_PERIOD 500
#define ORP_VAL_MAX 999
#define ORP_VAL_MIN -999
#define ORP_K_COEF (0.5f)

#ifdef ORP_DEBUG

#define ORPprint(a) dbSerial.print(a)
#define ORPprintln(a) dbSerial.println(a)

#else

#define ORPprint(a)
#define ORPprintln(a)

#endif

class ORPreading : public int32_p
{
private:
    bool Error;
    ExpFilter ORPmv;

public:
    int32_t GetVal(void);
    void Update(void);
    void Init(void);

};

#endif //__ORP_REAGING_H_