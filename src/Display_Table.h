
/*! \file Display_Table.h
    \brief Definuje zobrazení na displej
    \author Jan Kejík
*/
#ifdef Display_Table_includes

#include "TempReading.h"
#include "PHReading.h"
#include "ORPReading.h"
#include "ChangeIcon.h"
#include "pHControl.h"
#include "ORPControl.h"
#include "Flokulant.h"
#include "InfoMsg.h"
#include "TimeBlock.h"


int32_p phPar(70, 0, 140);
int32_p tepSetPar(0,-30,50);
int32_pe phSetPar(0,140);
int32_pe phDualState(0,1);
String_pe textTest(20);

extern int32_p motorTestTime;
extern Motor motor_pHplus;
extern Motor motor_pHminus;
extern Motor motor_floc;
extern Motor motor_chlor;


extern int32_pe phSetPoint;
extern int32_pe phAutoCtrlD;
extern int32_pe phAutoCtrlU;

extern pHManualControl manualpHCtrl;


Temp temp_01C;
extern AnalogReading TempRead;
extern AnalogReading VREFread;
extern AnalogReading PHRead;

extern PHReading pH_01;
extern int32_pe calphL_01;
extern int32_pe calphH_01;
extern CalpH calpH;

extern ORPreading orp;
extern AnalogReading ORPRead;


extern ChangeIcon OrpIcon;
extern ChangeIcon pHplusIcon;
extern ChangeIcon pHminusIcon;


extern int32_pe pumpOffTime; // minut
extern int32_pe pHDownDose; // mililitru/m3 na 1pH
extern int32_pe pHUPDose;   // mililitru/m3 na 1pH


extern ChangeIcon OrpIcon;
extern ORPManualControl manualORPCtrl;
extern int32_pe orpAutoCtrl;
extern int32_pe orpSetPoint;
extern int32_pe orpDose; 


extern ChangeIcon FlocIcon;
extern FlocManualControl manualFlocCtrl;
extern int32_pe flocAutoCtrl;
extern int32_pe flocSetPoint;

extern waveform phForm;

extern waveform phForm;
extern waveform tempForm;
extern waveform orpForm;

extern int32_pe poolVol; // metry krychlove


extern int32_pe WifiAP;
extern String_pe displaySSID;
extern ChangeIcon WifiIcon;
extern String_p OurTime;

extern String_p message[]; 

extern int32_p msgColor[];


extern int32_pe ErrorInfo;
extern int32_pe TempManual;
extern int32_pe TempAuto;
extern int32_pe nastTemperature; // metry krychlove/10

#else

// Intro stranka
begin_page(0)
set_page(1, 1)
end_page

// Uvodni stranka
begin_page(1)
disp_number("x0.val", temp_01C) 

disp_number("x1.val", pH_01)
disp_number("x2.val", phSetPoint)
disp_number("p4.pic", pHplusIcon)
disp_number("p5.pic", pHminusIcon)
set_page(17, 6)


disp_number("n1.val", orp)
disp_number("n2.val", orpSetPoint)
disp_number("p7.pic", OrpIcon)
set_page(24, 4)

set_page(19, 10)
disp_number("p8.pic", FlocIcon)
disp_number("n0.val", flocSetPoint)

disp_number("t1.txt", displaySSID)
disp_number("p3.pic", WifiIcon)
disp_number("t0.txt", OurTime)


disp_number("b3.pic", TimeIcon)

set_page(11, 11)
set_page(10, 12)
set_page(8, 2)
set_page(25, 14)
end_page

// Stranka nastaveni
begin_page(2)

set_number(2, "x0.val", poolVol, evnt_Sub, 1)
set_number(3, "x0.val", poolVol, evnt_Add, 1)

set_number(6, "bt0.val", WifiAP, evnt_Toggle, 1)

set_number(9, "n0.val", pumpOffTime, evnt_Sub, 1)
set_number(8, "n0.val", pumpOffTime, evnt_Add, 1)

set_page(1, 1)
set_page(12, 5)
end_page

// Stranka kalibrace pH
begin_page(3)
disp_number("x1.val", pH_01)
set_number(6, "x0.val", calpH, evnt_Sub, 1)
set_number(5, "x0.val", calpH, evnt_Add, 1)

set_number(11, "x0.val", calpH, evnt_Set, -1)
disp_number("x2.val", calphL_01)
disp_number("x3.val", calphH_01)

set_page(12, 1)
set_page(13, 8)
end_page



// Stranka kalibrace ORP
begin_page(4)

set_number(3, "bt0.val", orpAutoCtrl, evnt_Toggle, 1)

set_number(10, "p1.pic", manualORPCtrl, evnt_Moment, 1)

set_number(6, "n0.val", orpSetPoint, evnt_Sub, 5)
set_number(7, "n0.val", orpSetPoint, evnt_Add, 5)

set_page(2, 1)
set_page(12, 9)

end_page

// Stranka ADC hodnot
begin_page(5)
disp_number("n0.val", TempRead)
disp_number("n1.val", VREFread)
disp_number("n2.val", PHRead)
disp_number("n3.val", ORPRead)

set_page(4, 1)
set_page(16, 2)
set_page(15, 13)
end_page

// Regulace pH
begin_page(6)

set_number(12, "x0.val", phSetPoint, evnt_Sub, 1)
set_number(13, "x0.val", phSetPoint, evnt_Add, 1)

set_number(8, "p1.pic", manualpHCtrl, evnt_Moment, 2)
set_number(7, "p1.pic", manualpHCtrl, evnt_Moment, 1)


set_number(3, "bt0.val", phAutoCtrlD, evnt_Toggle, 1)
set_number(4, "bt1.val", phAutoCtrlU, evnt_Toggle, 1)
set_page(1, 1)
set_page(10, 8)
end_page


// Parametry regulace pH
begin_page(8)


set_number(3, "n1.val", pHUPDose, evnt_Sub, 1)
set_number(2, "n1.val", pHUPDose, evnt_Add, 1)

set_number(6, "n2.val", pHDownDose, evnt_Sub, 1)
set_number(5, "n2.val", pHDownDose, evnt_Add, 1)

set_page(1, 1)
set_page(12, 6)
set_page(11, 3)
end_page


// REDOX Kalibrace
begin_page(9)

set_number(4, "n1.val", orpDose, evnt_Sub, 1)
set_number(3, "n1.val", orpDose, evnt_Add, 1)

set_page(1, 1)
set_page(6, 4)
end_page

// Nastaveni vlockovace
begin_page(10)

set_number(3, "bt0.val", flocAutoCtrl, evnt_Toggle, 1)

set_number(10, "p1.pic", manualFlocCtrl, evnt_Moment, 1)

set_number(6, "n0.val", flocSetPoint, evnt_Sub, 1)
set_number(7, "n0.val", flocSetPoint, evnt_Add, 1)

set_page(2, 1)
end_page

// Graf
begin_page(11)

set_waveform(2, 0, phForm)
set_waveform(2, 1, tempForm)
set_waveform(2, 2, orpForm)
set_page(1, 1)
end_page

// Nastaveni vlockovace
begin_page(12)

disp_number("t1.txt", message[0])
disp_number("t1.pco", msgColor[0])

disp_number("t2.txt", message[1])
disp_number("t2.pco", msgColor[1])

disp_number("t3.txt", message[2])
disp_number("t3.pco", msgColor[2])

disp_number("t4.txt", message[3])
disp_number("t4.pco", msgColor[3])

disp_number("t5.txt", message[4])
disp_number("t5.pco", msgColor[4])

disp_number("t6.txt", message[5])
disp_number("t6.pco", msgColor[5])


set_number(9, "bt0.val", ErrorInfo, evnt_Toggle, 1)

set_page(2, 1)
end_page

// Testovani motoru
begin_page(13)

set_number(3, "n0.val", motorTestTime, evnt_Sub, 1)
set_number(4, "n0.val", motorTestTime, evnt_Add, 1)

set_number(8, "bt1.val", motor_pHplus, evnt_Set, 1)
set_number(7, "bt0.val", motor_pHminus, evnt_Set, 1)

set_number(9, "bt2.val", motor_chlor, evnt_Set, 1)
set_number(10, "bt3.val", motor_floc, evnt_Set, 1)

set_page(11, 5)
set_page(12, 22)
set_page(1, 1)
end_page


// Vypnutí automaticke regulace
begin_page(14)

set_number(2, "t2.txt", blockBeginStart, evnt_Sub, 300)
set_number(3, "t2.txt", blockBeginStart, evnt_Add, 300)

set_number(6, "t4.txt", blockBeginEnd, evnt_Sub, 300)
set_number(5, "t4.txt", blockBeginEnd, evnt_Add, 300)

set_page(1, 1)
set_page(9, 23)
end_page


// Regulace cerpadla
begin_page(15)


set_number(12, "bt0.val", CircManual, evnt_Toggle, 1)
set_page(2, 16)
set_page(6, 17)
set_page(7, 18)
set_page(8, 19)
set_page(9, 20)
set_page(10, 21)

set_page(1, 1)
set_page(4, 23)

end_page

// Nastaveni casovace 1
begin_page(16)

set_number(2, "t2.txt", CircOnTime, evnt_Sub, 300)
set_number(3, "t2.txt", CircOnTime, evnt_Add, 300)

set_number(6, "t4.txt", CircOffTime, evnt_Sub, 300)
set_number(5, "t4.txt", CircOffTime, evnt_Add, 300)

set_page(1, 1)
set_page(8, 15)

end_page

// Nastaveni casovace 2
begin_page(17)

set_number(2, "t2.txt", CircOnTime2, evnt_Sub, 300)
set_number(3, "t2.txt", CircOnTime2, evnt_Add, 300)

set_number(6, "t4.txt", CircOffTime2, evnt_Sub, 300)
set_number(5, "t4.txt", CircOffTime2, evnt_Add, 300)

set_page(1, 1)
set_page(8, 15)

end_page
// Nastaveni casovace 3
begin_page(18)

set_number(2, "t2.txt", CircOnTime3, evnt_Sub, 300)
set_number(3, "t2.txt", CircOnTime3, evnt_Add, 300)

set_number(6, "t4.txt", CircOffTime3, evnt_Sub, 300)
set_number(5, "t4.txt", CircOffTime3, evnt_Add, 300)

set_page(1, 1)
set_page(8, 15)

end_page
// Nastaveni casovace 4
begin_page(19)

set_number(2, "t2.txt", CircOnTime4, evnt_Sub, 300)
set_number(3, "t2.txt", CircOnTime4, evnt_Add, 300)

set_number(6, "t4.txt", CircOffTime4, evnt_Sub, 300)
set_number(5, "t4.txt", CircOffTime4, evnt_Add, 300)

set_page(1, 1)
set_page(8, 15)

end_page
// Nastaveni casovace 5
begin_page(20)

set_number(2, "t2.txt", CircOnTime5, evnt_Sub, 300)
set_number(3, "t2.txt", CircOnTime5, evnt_Add, 300)

set_number(6, "t4.txt", CircOffTime5, evnt_Sub, 300)
set_number(5, "t4.txt", CircOffTime5, evnt_Add, 300)

set_page(1, 1)
set_page(8, 15)

end_page
// Nastaveni casovace 6
begin_page(21)

set_number(2, "t2.txt", CircOnTime6, evnt_Sub, 300)
set_number(3, "t2.txt", CircOnTime6, evnt_Add, 300)

set_number(6, "t4.txt", CircOffTime6, evnt_Sub, 300)
set_number(5, "t4.txt", CircOffTime6, evnt_Add, 300)

set_page(1, 1)
set_page(8, 15)

end_page

// Nastaveni vykonu pump
begin_page(22)

set_number(3, "n0.val", pumpCapacityDez, evnt_Sub, 1)
set_number(4, "n0.val", pumpCapacityDez, evnt_Add, 1)

set_number(11, "n1.val", pumpCapacitypHplus, evnt_Sub, 1)
set_number(12, "n1.val", pumpCapacitypHplus, evnt_Add, 1)

set_number(14, "n2.val", pumpCapacitypHminus, evnt_Sub, 1)
set_number(15, "n2.val", pumpCapacitypHminus, evnt_Add, 1)

set_number(17, "n3.val", pumpCapacityFl, evnt_Sub, 1)
set_number(18, "n3.val", pumpCapacityFl, evnt_Add, 1)


set_page(1, 1)
set_page(7, 13)

end_page


// Regulace ohrevu
begin_page(23)

set_number(11, "bt1.val", TempManual, evnt_Toggle, 1)
set_number(6, "bt0.val", TempAuto, evnt_Toggle, 1)

set_number(7, "x0.val", nastTemperature, evnt_Sub, 1)
set_number(9, "x0.val", nastTemperature, evnt_Add, 1)

set_page(3, 14)
set_page(2, 15)
set_page(4, 1)

end_page


#endif


