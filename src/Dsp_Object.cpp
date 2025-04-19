#include "Dsp_object.h"

#define FIRST_PAGE 1 // Krome intra

uint32_t Dsp_Object::event_time;
const Event_Action_Def *Dsp_Object::pcfg_act = NULL;
bool Dsp_Object::auto_repeat = false;
const Event_Action_Def *Dsp_Object::pcfgSent = NULL;
uint8_t Dsp_Object::pageAct = 1;
uint16_t Dsp_Object::responseTimeout = 0;
// #define DISPLAY_DEBUG
#include "Buzzer.h"
void Dsp_Object::SendCommand(const char *cmd)
{
    while (nexSerial.available())
    {
        nexSerial.read();
    }

    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
}

bool Dsp_Object::recvRetCommandFinished(uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[4] = {0};

    nexSerial.setTimeout(timeout);
    if (sizeof(temp) != nexSerial.readBytes((char *)temp, sizeof(temp)))
    {
        ret = false;
    }

    if (temp[0] == NEX_RET_CMD_FINISHED && temp[1] == 0xFF && temp[2] == 0xFF && temp[3] == 0xFF)
    {
        ret = true;
    }

    if (ret)
    {
        dbSerialPrintln("recvRetCommandFinished ok");
    }
    else
    {
        dbSerialPrintln("recvRetCommandFinished err");
    }

    return ret;
}

void DebugEvent(const Event_Action_Def *pcfg, String msg)
{
#ifdef DISPLAY_DEBUG
    dbSerialPrint(msg);
    dbSerialPrint(" :");
    switch (pcfg->evea)
    {
    case evnt_Add:
        dbSerialPrint("Add: ");
        break;
    case evnt_Sub:
        dbSerialPrint("Sub: ");
        break;
    case evnt_Set:
        dbSerialPrint("Set: ");
        break;
    case evnt_Get:
        dbSerialPrint("Get: ");
        break;
    case evnt_Set_Page:
        dbSerialPrint("Set_Page: ");
        break;
    case evnt_Disp:
        dbSerialPrint("Change: ");
        break;

    case evnt_Toggle:
        dbSerialPrint("Toggle: ");
        break;
    case evnt_Moment:
        dbSerialPrint("Momentary: ");
        break;

    case evnt_Wave:
        dbSerialPrint("Waveform: ");
        break;

    default:
        dbSerialPrint("Unknown: ");
        break;
    }
#endif
}

bool Dsp_Object::Process_Event(const Event_Action_Def *pcfg, uint8_t butt_id, uint32_t event)
{
    bool retval = false;
    if (pcfg != NULL)
    {

        if (butt_id == pcfg->butt_id)
        {
            buzzer.Beep(1, 50);
            if (event == BUTT_ON_EVENT)
            {
                if (Param_Change(pcfg))
                {
                    DebugEvent(pcfg_act, "Event ON");
                    auto_repeat = false;
                    retval = true;
                }
                else
                {
                    pcfg_act = NULL;
                }
            }
            else if (event == BUTT_OFF_EVENT)
            {

                if (pcfg->evea == evnt_Get || pcfg->evea == evnt_Set_Page || pcfg->evea == evnt_Moment)
                {
                    pcfg_act = pcfg;
                    if (pcfg->evea == evnt_Moment)
                    {
                        pcfg_act->dispParam->SetVal(0);
                    }
                    DebugEvent(pcfg_act, "Event OFF");
                    auto_repeat = false;
                    retval = true;
                }
                else
                {
                    pcfg_act = NULL;
                }
            }
        }
    }
    return retval;
}
bool Dsp_Object::Param_Change(const Event_Action_Def *pcfg)
{
    if (pcfg != NULL)
    {
        event_time = millis();
        pcfg_act = pcfg;
        switch (pcfg_act->evea)
        {
        case evnt_Add:
            pcfg_act->dispParam->SetVal(pcfg_act->dispParam->GetVal() + pcfg_act->setVal);
            dbDispObj("+");
            break;
        case evnt_Sub:
            pcfg_act->dispParam->SetVal(pcfg_act->dispParam->GetVal() - pcfg_act->setVal);
            dbDispObj("-");
            break;
        case evnt_Set:
        case evnt_Moment:
            pcfg_act->dispParam->SetVal(pcfg_act->setVal);
            dbDispObj("=");
            break;
        case evnt_Toggle:
            if (pcfg_act->dispParam->GetVal() != 0)
            {
                pcfg_act->dispParam->SetVal(0);
            }
            else
            {
                pcfg_act->dispParam->SetVal(pcfg_act->setVal);
            }
            break;
        default:
            pcfg_act = NULL;
            break;
        }
    }
    return pcfg_act != NULL;
}
bool Dsp_Object::AutoRepeat(void)
{
    bool retval = false;
    if (pcfg_act != NULL)
    {
        if (pcfg_act->evea == evnt_Add || pcfg_act->evea == evnt_Sub)
        {
            uint32_t tr = (auto_repeat) ? (AUTO_REPEAT_SPEED) : (AUTO_REPEAT_START);
            if ((millis() - event_time) > (tr))
            {
                Param_Change(pcfg_act);
                auto_repeat = true;
            }
        }
    }
    return retval;
}

bool Dsp_Object::Value_Send(const Event_Action_Def *pcfg)
{
    bool retval = false;
    String number, cmd;

    if (pcfgSent == NULL)
    {
        responseTimeout = 200;
        if (pcfg_act != NULL)
        {
            switch (pcfg_act->evea)
            {
            case evnt_Add:
            case evnt_Sub:
            case evnt_Set:
            case evnt_Toggle:
                if (pcfg_act->dispParam->changed())
                {
                    pcfgSent = pcfg_act;
                    DebugEvent(pcfgSent, "Posilam aktualni");
                    pcfgSent->dispParam->GetString(number);
                    cmd = pcfgSent->disp_id;
                    cmd += "=";
                    cmd += number;
                    dbDispObj(cmd);
                    SendCommand(cmd.c_str());
                    retval = true;
                }
                break;

            case evnt_Get:
                pcfgSent = pcfg_act;
                DebugEvent(pcfgSent, "Posilam");
                cmd = "get ";
                cmd += pcfgSent->disp_id;
                // cmd += "val";
                cmd += number;
                dbDispObj(cmd);
                SendCommand(cmd.c_str());
                retval = true;
                pcfg_act = NULL;
                break;

            case evnt_Set_Page:
                pcfgSent = pcfg_act;
                if (pcfgSent->setVal != pageAct)
                {
                    Set_Page((uint8_t)pcfgSent->setVal);
                    retval = true;
                    pcfg_act = NULL;
                }
                break;

            default:
                break;
            }
        }
        if (pcfgSent == NULL)
        {
            if (pcfg->dispParam != NULL)
            {
                if (pcfg->dispParam->changed())
                {
                    if (pcfg->evea == evnt_Wave)
                    {

                        if (pcfg->dispParam->GetSize() != 0)
                        {
                            pcfg->dispParam->GetString(number);
                            pcfgSent = pcfg;
                            DebugEvent(pcfgSent, "Start");
                            cmd = pcfgSent->disp_id;
                            cmd += number;
                            dbDispObj(cmd);
                            SendCommand(cmd.c_str());
                            retval = true;
                        }
                    }
                    else
                    {
                        pcfgSent = pcfg;
                        DebugEvent(pcfgSent, "Posilam aktualni");
                        pcfgSent->dispParam->GetString(number);
                        cmd = pcfgSent->disp_id;
                        cmd += "=";
                        cmd += number;
                        dbDispObj(cmd);
                        SendCommand(cmd.c_str());
                        retval = true;
                    }
                }
            }
        }
    }
    else
    {
        if (responseTimeout != 0)
        {
            responseTimeout--;
            if (responseTimeout == 0)
            {
                pcfgSent = NULL;
            }
        }
        
    }

    return retval;
}

void Dsp_Object::Transparent_Data_Send(void)
{
    if (pcfgSent != NULL)
    {
        uint8_t dt;
        while (pcfgSent->dispParam->GetData(dt))
        {
            dbDispObj(dt);
            nexSerial.write(dt);
        }
    }
}

bool Dsp_Object::Set_Page(uint8_t page)
{

    bool retval = true;
    String cmd = "page ";
    cmd += page;
    dbDispObj(cmd);
    SendCommand(cmd.c_str());
    return retval;
}

bool Dsp_Object::Value_Get(int32_t val)
{
    bool retval = false;
    if (pcfgSent != NULL)
    {
        if (pcfgSent->evea == evnt_Get)
        {
            dbDispObj("prijato");
            pcfgSent->dispParam->SetVal(val);
            pcfgSent = NULL;
            retval = true;
        }
    }
    return retval;
}

bool Dsp_Object::Is_Page_Change(uint8_t *page)
{
    if (pcfgSent->evea == evnt_Set_Page)
    {
        *page = (uint8_t)pcfgSent->setVal;

        dbSerialPrint("IsPage");
        dbDispObj(*page);
        return true;
    }

    return false;
}

bool Dsp_Object::Value_Set(void)
{

    if (pcfgSent != NULL)
    {

        if (pcfgSent->evea == evnt_Set_Page)
        {
            pageAct = pcfgSent->setVal;
        }
        pcfgSent = NULL;
    }

    // dbDispObj("OK");
    return true;
}

bool Dsp_Object::Value_Send_All(const Event_Action_Def *pcfg)
{
    while (pcfg->evea != evnt_None)
    {
        if (pcfg->dispParam != NULL)
        {
            pcfg->dispParam->SetChange();
        }
        pcfg++;
    }
    return true;
}
void Dsp_Object::Value_Error(void)
{
    pcfgSent = NULL;
}