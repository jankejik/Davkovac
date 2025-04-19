#include "ChangeIcon.h"

bool ChangeIcon::SetVal(int32_t value)
{
    if (selected != value)
    {
        selected = (uint16_t)value;
        change = true;
    }
    return true;
}

void ChangeIcon::Update(void)
{
    if (icons[selected] & BLINK)
    {
        flash = !flash;
        change = true;
    }
}

void ChangeIcon::GetString(String &retVal)
{
    uint32_t temp = icons[selected];
    if ((((temp & BLINK) == 0) || flash) && ((temp & ~BLINK) != INVALID_ICON))
    {
        retVal = temp & ~BLINK;
    }
    else
    {
        retVal = icons[0] & ~BLINK;
    }
    change = false;
}