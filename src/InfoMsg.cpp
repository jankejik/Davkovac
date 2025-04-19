#include "InfoMsg.h"
#include "string.h"
#include "Web.h"

#define ERROR_COLLOR 65504
#define INFO_COLLOR 1885

String_p message[MSG_LINE_NUMBER] = {String_p(MSG_LINE_SIZE), String_p(MSG_LINE_SIZE), String_p(MSG_LINE_SIZE), String_p(MSG_LINE_SIZE), String_p(MSG_LINE_SIZE), String_p(MSG_LINE_SIZE)};
int32_p msgColor[MSG_LINE_NUMBER] = {int32_p(ERROR_COLLOR, 0, 65535), int32_p(ERROR_COLLOR, 0, 65535), int32_p(ERROR_COLLOR, 0, 65535),
                                     int32_p(ERROR_COLLOR, 0, 65535), int32_p(ERROR_COLLOR, 0, 65535)};

WebInfoMsg webTxtLogs;

String WebInfoMsg::message[WEB_MSG_LINE_NUMBER];
size_t WebInfoMsg::nmrLines = 0;
String WebInfoMsg::txtLog = "";

int32_pe ErrorInfo(0, 1);
uint8_t InfoMsg::actLine = 0;
char InfoMsg::lineBuff[MSG_LINE_SIZE + 1];
char WebInfoMsg::lineBuff[WEB_MSG_LINE_SIZE + 1];
int32_t InfoMsg::actColor = 65535;

void InfoMsg::Init(void)
{
    ErrorInfo.Init(0);
}

size_t InfoMsg::WriteLine(char *ptr)
{
    if (actLine < MSG_LINE_NUMBER)
    {
        message[actLine] = lineBuff;
        msgColor[actLine] = actColor;
        actLine++;
    }
    else
    {
        for (int i = 0; i < actLine - 1; i++)
        {
            message[i] = message[i + 1];
            msgColor[i].SetVal(msgColor[i + 1].GetVal());
        }
        message[MSG_LINE_NUMBER - 1] = lineBuff;
        msgColor[MSG_LINE_NUMBER - 1] = actColor;
    }

    return 0;
}

void TimeStamp(void)
{
    sprintf(InfoMsg::lineBuff, "%02d:%02d  ", hour(), minute());
}

void WebLog(const char *type, const char *format, ...)
{

    va_list args;
    va_start(args, format);

    size_t len = snprintf(webTxtLogs.lineBuff, WEB_MSG_LINE_SIZE, "%02d:%02d\t%s: ", hour(), minute(), type);
    vsnprintf(webTxtLogs.lineBuff + len, WEB_MSG_LINE_SIZE - len, format, args);
    webTxtLogs.WriteLine(webTxtLogs.lineBuff);

    va_end(args);
}

void Msg(const char *format, ...)
{
    va_list args;
    
    va_start(args, format);
    
    InfoMsg::actColor = INFO_COLLOR;
    TimeStamp();
    vsnprintf(InfoMsg::lineBuff + TIME_STAMP_SIZE, MSG_LINE_SIZE - TIME_STAMP_SIZE, format, args);
    if (!ErrorInfo)
    {
        InfoMsg::WriteLine(InfoMsg::lineBuff);
    }
    
    va_end(args);
  
}

void Err(const char *format, ...)
{
    InfoMsg::actColor = ERROR_COLLOR;
    va_list args;
    va_start(args, format);
    TimeStamp();
    vsnprintf(InfoMsg::lineBuff + TIME_STAMP_SIZE, MSG_LINE_SIZE - TIME_STAMP_SIZE, format, args);
    InfoMsg::WriteLine(InfoMsg::lineBuff);

    va_end(args);

}

void WebInfoMsg::Init(void)
{
    nmrLines = 0;
}

size_t WebInfoMsg::WriteLine(char *ptr)
{
    if (nmrLines >= WEB_MSG_LINE_NUMBER)
    {
        String tmp=txtLog;
        tmp.trim();
        txtLog.remove(tmp.lastIndexOf("\n\r"));
        /*
        size_t endFirst = txtLog.indexOf("\n\r");
        if (endFirst > 0)
        {
            txtLog.remove(0, endFirst + 1);
        }*/
    }
    else
    {
        nmrLines++;
    }
    String temp = ptr;
    temp += "\n\r";
    txtLog = temp + txtLog;
    //txtLog += "\n\r";

    return 0;
}
