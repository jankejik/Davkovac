/*! \file Display.cpp
    \brief Ridi komunikaci s displejem.
    \author Jan Kejik
*/
#include "Param.h"
#include "Dsp_object.h"
#include "Display.h"
#include "Common.h"

#define Display_Table_includes
#include "Display_Table.h"
#undef Display_Table_includes
#define str(txt) #txt
#define begin_page(_id) const Event_Action_Def event_list##_id[] = {

#define set_number(_butt, _dispName, _par, _event, _val) {_event, _butt, &_par, _val, _dispName},
#define disp_number(_dispName, _par) {evnt_Disp, INVALID_TOUCH_ID, &_par, 0, _dispName},
#define set_page(_buttonId, _page) {evnt_Set_Page, _buttonId, NULL, _page, ""},
#define set_waveform(_Id, _ch, _par) {evnt_Wave, _Id, &_par, 0, "addt " str(_Id) str(\x2C) str(_ch) str(\x2C)},
#define end_page                                 \
    {                                            \
        evnt_None, INVALID_TOUCH_ID, NULL, 0, "" \
    }                                            \
    }                                            \
    ;

#include "Display_Table.h"

#undef begin_page
#undef set_number
#undef disp_number
#undef set_page
#undef set_waveform
#undef end_page

#define begin_page(_id) {_id, event_list##_id},
#define set_number(_butt, _dispName, _par, _event, _val)
#define disp_number(_dispName, _par)
#define set_page(_buttonId, _page)
#define set_waveform(_Id, _ch, _par)
#define end_page

Page_Display_Def page_display[] = {
#include "Display_Table.h"
    {INVALID_PAGE, NULL}};

uint8_t Display::page_act = INVALID_PAGE;
uint8_t Display::buffer[10];
uint8_t Display::cmd = INVALID_CMD;
Page_Display_Def *Display::object_act = NULL;
uint32_t Display::lastTouchTime = 0;

bool Display::Init(void)
{
    bool ret1 = false;
    bool ret2 = false;

    dbSerial.begin(115200);
    nexSerial.begin(115200);
    Dsp_Object::SendCommand("");
    Dsp_Object::SendCommand("bkcmd=1");
    ret1 = Dsp_Object::recvRetCommandFinished();
    Dsp_Object::SendCommand("page 0");
    ret2 = Dsp_Object::recvRetCommandFinished();
    lastTouchTime = millis();
    return ret1 && ret2;
}

void Display::Intro(void)
{
    String temp;
    temp = "t2.txt=";
    temp += "\"Verze ";
    temp += VERZE_MAJOR;
    temp += ".";
    temp += VERZE_MINOR;
    temp += "    ";
    temp += CMP_DATE;
    temp += "\"";
    Dsp_Object::SendCommand(temp.c_str());
    WebLog("Info","Verze: %d.%d datum kompilace %s", VERZE_MAJOR, VERZE_MINOR, CMP_DATE);
    delay(5000);
    Dsp_Object::Set_Page(1);
}

bool Display::Select_Page(uint8_t page)
{
    if (object_act == NULL || object_act->page != page)
    {
        object_act = page_display;

        while (object_act->page != INVALID_PAGE)
        {

            if (object_act->page == page)
            {
                Dsp_Object::Value_Send_All(object_act->object_list);
                break;
            }
            object_act++;
        }
        dbSerialPrint("Prepinam obrazovku: ");
        dbSerialPrintln(object_act->page);
    }

    return page_act != INVALID_PAGE;
}

bool Display::isEnd(void)
{
    if (nexSerial.read() == 0xFF)
    {
        if (nexSerial.read() == 0xFF)
        {
            if (nexSerial.read() == 0xFF)
            {
                return true;
            }
        }
    }
    return false;
}

void Display::Process_Reception(void)
{
    if (cmd == INVALID_CMD)
    {
        if (nexSerial.available() > 0)
        {
            cmd = nexSerial.read();
        }
    }

    switch (cmd)
    {
    case NEX_RET_CMD_FINISHED:
        if (nexSerial.available() >= 3)
        {
            if (isEnd())
            {
                // dbSerialPrintln("OK");
                Dsp_Object::Value_Set();
            }
            else
            {
                Dsp_Object::Value_Error();
                dbSerialPrintln("Invalid response!!");
            }
            cmd = INVALID_CMD;
        }

        break;
    case NEX_RET_EVENT_TOUCH_HEAD:

        if (nexSerial.available() >= 6)
        {
            uint8_t pageid;
            uint8_t touchid;
            int32_t status;
            lastTouchTime = millis();
            pageid = nexSerial.read();
            touchid = nexSerial.read();
            status = (int32_t)nexSerial.read();

            if (isEnd())
            {

                if (object_act != NULL)
                {
                    if (object_act->page == pageid)
                    {
                        const Event_Action_Def *object = object_act->object_list;
                        while (object->evea != evnt_None)
                        {

                            if (Dsp_Object::Process_Event(object, touchid, status))
                            {
                                dbSerialPrintln(object->butt_id);

                                break;
                            }

                            object++;
                        }
                    }
                    else
                    {
                        Dsp_Object::pageAct = pageid;
                    }
                }
            }
            cmd = INVALID_CMD;
        }
        break;

    case NEX_RET_NUMBER_HEAD:

        if (nexSerial.available() >= 7)
        {
            uint8_t num[4];
            int32_t val;
            num[0] = nexSerial.read();
            num[1] = nexSerial.read();
            num[2] = nexSerial.read();
            num[3] = nexSerial.read();

            val = (uint32_t)num[0] | (uint32_t)num[1] << 8 | (uint32_t)num[2] << 16 | (uint32_t)num[3] << 24;

            if (isEnd())
            {
                Dsp_Object::Value_Get(val);
                dbSerialPrintln("RX");
                dbSerialPrintln(val);
            }
            else
            {
                dbSerialPrintln("Invalid response!!");
            }
            cmd = INVALID_CMD;
        }
        break;
    case INVALID_CMD:
    case NEX_RET_INVALID_PAGE_ID:
    case NEX_RET_INVALID_PICTURE_ID:
    case NEX_RET_INVALID_FONT_ID:
    case NEX_RET_INVALID_BAUD:
    case NEX_RET_INVALID_OPERATION:
    case NEX_RET_INVALID_VARIABLE:
        if (nexSerial.available() >= 3)
        {
            isEnd();
            dbSerialPrintln("Invalid variable!!");
            Dsp_Object::Value_Error();
            cmd = INVALID_CMD;
        }
        break;

    case NEX_RET_TRANSPARENT_START:
        if (nexSerial.available() >= 3)
        {
            if (isEnd())
            {
                dbSerialPrintln("Transparent data start!");
                Dsp_Object::Transparent_Data_Send();
            }
            else
            {
                Dsp_Object::Value_Error();
                dbSerialPrintln("Invalid response!!");
            }
            cmd = INVALID_CMD;
        }
        break;

    case NEX_RET_TRANSPARENT_END:
        if (nexSerial.available() >= 3)
        {
            if (isEnd())
            {
                Dsp_Object::Value_Error();
                dbSerialPrintln("Transparent data end!");
            }
            else
            {
                Dsp_Object::Value_Error();
                dbSerialPrintln("Invalid response!!");
            }
            cmd = INVALID_CMD;
        }
        break;
    default:
        Dsp_Object::Value_Error();
        cmd = INVALID_CMD;
        break;
    }
}

void Display::Run(void)
{
    if (Dsp_Object::pageAct != 1)
    {
        if ((millis() - lastTouchTime) > (DISPLAY_IN_ACTIVITY_SEC * 1000))
        {
           Dsp_Object::Set_Page(1);
            lastTouchTime = millis();
        }
    }
    else
    {
        lastTouchTime = millis();
    }

    Select_Page(Dsp_Object::pageAct);
    if (object_act != NULL)
    {
        const Event_Action_Def *object = object_act->object_list;
        Dsp_Object::AutoRepeat();
        while (object->evea != evnt_None)
        {
            if (Dsp_Object::Value_Send(object))
            {
                break;
            }
            object++;
        }
    }
}