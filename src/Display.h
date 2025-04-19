

/*! \file Display.h
    \author Jan Kejík
*/

#include "Arduino.h"
#include "Common.h"

#ifndef __DISPLAY_H_
#define __DISPLAY_H_


#define NEXTION_READING 1
#define NEXTION_WRITING 20
#define INVALID_PAGE 255
#define START_PAGE 0
#define INVALID_CMD 255
#define INVALID_TOUCH_ID 255
#define DISPLAY_IN_ACTIVITY_SEC (15*60)


#define NEX_RET_EVENT_LAUNCHED          (0x88)
#define NEX_RET_EVENT_UPGRADED          (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)     
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_INVALID_CMD             (0x00)
#define NEX_RET_INVALID_COMPONENT_ID    (0x02)
#define NEX_RET_INVALID_PAGE_ID         (0x03)
#define NEX_RET_INVALID_PICTURE_ID      (0x04)
#define NEX_RET_INVALID_FONT_ID         (0x05)
#define NEX_RET_INVALID_BAUD            (0x11)
#define NEX_RET_INVALID_VARIABLE        (0x1A)
#define NEX_RET_INVALID_OPERATION       (0x1B)
#define NEX_RET_TRANSPARENT_START       (0xFE)
#define NEX_RET_TRANSPARENT_END         (0xFD)




typedef struct 
{
    uint8_t page;
    const Event_Action_Def * object_list;

} Page_Display_Def;

class Display
{
    private:
    static bool writeResponse;
    static uint8_t buffer[10];
    static bool isEnd(void);
    static uint8_t cmd;
    static uint32_t lastTouchTime;

    protected:
    static uint8_t page_act;
    static Page_Display_Def * object_act;

    public:    
    //void SetPage(uint8_t page);
    //uint8_t GetPage(void);
    static void Process_Reception(void);
    static void Run(void);
    static bool Select_Page(uint8_t page);
    static bool Init(void);
    static void Intro(void);
    
    
};


#endif //__DISPLAY_H_