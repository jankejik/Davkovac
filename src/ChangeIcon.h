/*! \file ChangeIcon.h
    \author Jan Kejík
*/


#include "Arduino.h"
#include "Param.h"
#include "Common.h"

#ifndef __CHANGE_ICON_H_
#define __CHANGE_ICON_H_

#define BLINK 0x8000
#define NUMBER_ICON 4
#define INVALID_ICON 0x7FFF

class ChangeIcon : public param
{
private:
    bool flash;
    uint16_t selected;
    uint16_t icons[NUMBER_ICON];

public:
    ChangeIcon(uint16_t baseIcon, uint16_t iconSel1, uint16_t iconSel2 = INVALID_ICON, uint16_t iconSel3 = INVALID_ICON)
    {
        icons[0] = baseIcon;
        icons[1] = iconSel1;
        icons[2] = iconSel2;
        icons[3] = iconSel3;
    }
    int32_t GetVal(void)
    {
        return selected;
    }
     bool SetVal(int32_t value);
    void Update(void);
    void GetString(String &retVal);

    ChangeIcon &operator=(int32_t in)
    {
        SetVal(in);
        return *this;
    }
};

#endif //__CHANGE_ICON_H_