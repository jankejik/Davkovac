/*! \file InfoMsg.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "Param.h"
#include <stdio.h>
#include "Common.h"

#ifndef __INFO_MSG_H_
#define __INFO_MSG_H_

#define MSG_LINE_SIZE 33
#define MSG_LINE_NUMBER 6

#define WEB_MSG_LINE_NUMBER 50
#define WEB_MSG_LINE_SIZE 90

#define TIME_STAMP_SIZE 6

extern String_p message[];

class InfoMsg
{
private:
    static uint8_t nmrLines;
    static uint8_t actLine;
    static uint8_t linePoz;

public:
    static int32_t actColor;
    char timeStamp[6];
    static size_t WriteLine(char *ptr);
    static char lineBuff[MSG_LINE_SIZE + 1];

    static void Init(void);
};

class WebInfoMsg : public param
{
private:
    static size_t nmrLines;

public:
    static String message[WEB_MSG_LINE_NUMBER];
    static String txtLog;
    char timeStamp[6];
    static size_t WriteLine(char *ptr);
    static char lineBuff[WEB_MSG_LINE_SIZE + 1];
    void GetString(String &retVal)
    {
        retVal = txtLog;
    }
    static void Init(void);
};
void WebLog(const char *type, const char *format, ...);

void Err(const char *format, ...);
void Msg(const char *format, ...);
#endif //__INFO_MSG_H_
