/*! \file main.cpp
    \author Jan Kejík
*/

#include <Arduino.h>
#include "Common.h"
#include "EEPROM.h"
#include "Dsp_object.h"
#include "TaskSchedulerDeclarations.h"
#include "TaskScheduler.h"
#include "Display.h"
#include "TempReading.h"
#include "AnalogReading.h"
#include "PHReading.h"
#include "ORPReading.h"
#include "MotorControl.h"
#include "ChangeIcon.h"
#include "pHControl.h"
#include "ORPControl.h"
#include "Flokulant.h"
#include "Web.h"
#include "InfoMsg.h"
#include "TimeBlock.h"
#include "Buzzer.h"
#define DATA_LOCK_PERIOD 60000 // Posilani dat do grafu

#define DATA_LOCK_PERIOD_1 15000 // Posilani dat do grafu

#define DATA_LOCK_PERIOD_24 360000 // Posilani dat do grafu

#define WIFI_RECCONECT 150000

using namespace std;

extern Temp temp_01C;
extern String_pe textTest;
extern int32_p tepPar;
extern int32_p phPar;
extern PHReading pH_01;
extern ORPreading orp;
extern ChangeIcon OrpIcon;

extern pHManualControl manualpHCtrl;
extern ORPManualControl manualORPCtrl;
extern FlocManualControl manualFlocCtrl;

extern ChangeIcon OrpIcon;

extern ChangeIcon pHplusIcon;
extern ChangeIcon pHminusIcon;

waveform phForm(240, 150);
waveform tempForm(240, 150);
waveform orpForm(240, 150);

waveform phForm1(240, 150);
waveform tempForm1(240, 150);
waveform orpForm1(240, 150);

waveform phForm24(240, 150);
waveform tempForm24(240, 150);
waveform orpForm24(240, 150);

extern void BuildTime(void);

void TemperatureReading(void)
{
    temp_01C.Temp_Reading();
}

void phReading(void)
{
    pH_01.Update();
}

void orpReading(void)
{
    orp.Update();
}

void DisplayIkon(void)
{
    OrpIcon.Update();
    pHplusIcon.Update();
    pHminusIcon.Update();
    manualpHCtrl.Update();
    manualORPCtrl.Update();
    OrpIcon.Update();
    manualFlocCtrl.Update();
}
void DataLoging(void)
{
    tempForm.SetVal(temp_01C.GetVal() / 3);
    phForm.SetVal((pH_01.GetVal() - 60) * 75 / 10);
    orpForm.SetVal(orp.GetVal() / 6);
}

void DataLoging1(void)
{
    tempForm1.SetVal(temp_01C.GetVal() / 3);
    phForm1.SetVal((pH_01.GetVal() - 60) * 75 / 10);
    orpForm1.SetVal(orp.GetVal() / 6);
}

void DataLoging24(void)
{
    tempForm24.SetVal(temp_01C.GetVal() / 3);
    phForm24.SetVal((pH_01.GetVal() - 60) * 75 / 10);
    orpForm24.SetVal(orp.GetVal() / 6);
}

void Webtask(void)
{
    WifiMode();
}

void BuzzerUpdate(void)
{
    buzzer.Update();
}

Scheduler ts;

Task dspReading(NEXTION_READING *TASK_MILLISECOND, -1, Display::Process_Reception, &ts, true);
Task dspWriting(NEXTION_WRITING *TASK_MILLISECOND, -1, Display::Run, &ts, true);

Task TempReading(TEMP_READING_PERIOD *TASK_MILLISECOND, -1, TemperatureReading, &ts, true);
Task AnalogReading(ANALOG_READING_PERIOD *TASK_MILLISECOND, -1, AnalogReading::Reading_Task, &ts, true);
Task PHreading(PH_READING_PERIOD *TASK_MILLISECOND, -1, phReading, &ts, true);
Task ORPreading(ORP_UPDATE_PERIOD *TASK_MILLISECOND, -1, orpReading, &ts, true);
Task DisplayIcon(FLASH_UPDATE_PERIOD *TASK_MILLISECOND, -1, DisplayIkon, &ts, true);

Task MotorCtrl(MOTOR_UPDATE_PERIOD *TASK_MILLISECOND, -1, Motor::CtrlTask, &ts, true);
Task pHCtrl(PH_CTRL_CALL_PERIOD *TASK_MILLISECOND, -1, pHControl::pHAutocontrol, &ts, true);
Task orpCtrl(ORP_CTRL_CALL_PERIOD *TASK_MILLISECOND, -1, ORPControl::Autocontrol, &ts, true);
Task flocCtrl(FLOC_CTRL_CALL_PERIOD *TASK_MILLISECOND, -1, Flokulant::Autocontrol, &ts, true);

Task DataLock(DATA_LOCK_PERIOD *TASK_MILLISECOND, -1, DataLoging, &ts, true);
Task DataLock1(DATA_LOCK_PERIOD_1 *TASK_MILLISECOND, -1, DataLoging1, &ts, true);
Task DataLock24(DATA_LOCK_PERIOD_24 *TASK_MILLISECOND, -1, DataLoging24, &ts, true);

Task TimeLock(TIME_LOAD_PERIOD *TASK_MILLISECOND, -1, BuildTime, &ts, true);
Task WifiLook(WIFI_LOAD_PERIOD *TASK_MILLISECOND, -1, Webtask, &ts, true);

Task TimeBlock(TIME_BLOCK_PERIOD *TASK_MILLISECOND, -1, TimeBlockTask, &ts, true);

Task Wifirec(WIFI_RECCONECT *TASK_MILLISECOND, -1, WifiReconnect, &ts, true);

Task BuzzUpdate(BUZZER_READING *TASK_MILLISECOND, -1, BuzzerUpdate, &ts, true);

void setup(void)
{

    Display::Init();

    param::eep.begin(512);
    AnalogReading::Init();
    temp_01C.Init();
    pH_01.Init();
    InfoMsg::Init();
    Motor::InitAll();
    pHControl::Init();
    ORPControl::Init();
    Flokulant::Init();
    TimeInit();
    Display::Intro();
    orp.Init();
    buzzer.Init();
    WebInit();

    dbSerialPrintln("setup done");
}

void loop(void)
{
    ts.execute();
}