
#include "Param.h"
#include "Web.h"
#include "PHReading.h"
#include "ORPReading.h"
#include "TimeBlock.h"
#include "InfoMsg.h"

size_t param::lastEepAdr = 0;
EEPROMClass param::eep;

extern String_pe SSID;
extern String_pe Password;
extern int32_pe phSetPoint;

extern int32_pe phSetPoint;
extern int32_pe pHDownDose; // mililitru/m3 na 1pH
extern int32_pe pHUPDose;   // mililitru/m3 na 1pH

extern ORPreading orp;
extern int32_pe orpSetPoint;
extern int32_pe orpDose;

extern int32_pe flocSetPoint;

extern int32_pe pumpOffTime; // minut

extern int32_pe poolVol; // metry krychlove

extern int32_pe phAutoCtrlD;
extern int32_pe phAutoCtrlU;
extern int32_pe orpAutoCtrl;
extern int32_pe flocAutoCtrl;

extern WebInfoMsg webTxtLogs;

extern String_p OurTime;

typedef struct
{
    String jmeno;
    param *par;
    float readCoef;
} ParName;
#define STRING_F NAN
#define TIME_F 2

const ParName parDef[] = {
    {"ssid", &SSID, STRING_F},
    {"password", &Password, STRING_F},
    {"ph", &phSetPoint, 0.1},
    {"naspool", &poolVol, 0.1},
    {"nasTep", &nastTemperature, 0.1},
    {"naschlor", &orpDose, 1},
    {"nasminus", &pHDownDose, 1},
    {"nasplus", &pHUPDose, 1},
    {"nasvloc", &flocSetPoint, 1},
    {"nastorp", &orpSetPoint, 1},
    {"nasoff", &pumpOffTime, 1},
    {"phplus", &phAutoCtrlU, 1},
    {"phminus", &phAutoCtrlD, 1},
    {"chlor", &orpAutoCtrl, 1},
    {"floku", &flocAutoCtrl, 1},
    {"blockStart", &blockBeginStart, TIME_F},
    {"blockStop", &blockBeginEnd, TIME_F},
    {"CircOn", &CircOnTime, TIME_F},
    {"CircOff", &CircOffTime, TIME_F},
    {"CircOn2", &CircOnTime2, TIME_F},
    {"CircOff2", &CircOffTime2, TIME_F},
    {"CircOn3", &CircOnTime3, TIME_F},
    {"CircOff3", &CircOffTime3, TIME_F},
    {"CircOn4", &CircOnTime4, TIME_F},
    {"CircOff4", &CircOffTime4, TIME_F},
    {"CircOn5", &CircOnTime5, TIME_F},
    {"CircOff5", &CircOffTime5, TIME_F},
    {"CircOn6", &CircOnTime6, TIME_F},
    {"CircOff6", &CircOffTime6, TIME_F},
    {"HeatOn", &HeatOnTime, TIME_F},
    {"TempOff", &HeatOffTime, TIME_F},
    {"infoweb", &webTxtLogs, STRING_F},
};

#define NUMBER_PAR (sizeof(parDef) / sizeof(parDef[0]))

bool param::PutWebString(const String &name, const String &hodnota)
{
    // dbSerialPrint(name);
    // dbSerialPrint(" = ");
    // dbSerialPrint(hodnota);
    for (int i = 0; i < NUMBER_PAR; i++)
    {
        if (name == parDef[i].jmeno)
        {
            WEBprint("Web par: ");
            WEBprint(name);

            if (isnan(parDef[i].readCoef))
            {
                parDef[i].par->SetVal(hodnota);

                WEBprint(" TxtHodnota: ");
                WEBprintln(hodnota);
            }
            else if (parDef[i].readCoef == 1)
            {

                WEBprint(" NumIHodnota: ");
                int32_t temp = hodnota.toInt();
                WEBprintln(temp);
                parDef[i].par->SetVal(temp);
            }
            else if (parDef[i].readCoef == TIME_F)
            {
                if (hodnota == "")
                {
                    return true;
                }
                WEBprint(" TimeIHodnota: ");
                int idx = hodnota.indexOf(":");
                String th = hodnota.substring(0, idx);
                String tm = hodnota.substring(idx + 1);
                int32_t tempH = th.toInt();
                int32_t tempM = tm.toInt();
                int32_t temp = tempH * MIN_PER_HOUR * SEC_PER_MIN + tempM * SEC_PER_MIN;
                WEBprintln(temp);
                parDef[i].par->SetVal(temp);
            }

            else
            {
                WEBprint(" NumFHodnota: ");
                float temp = hodnota.toFloat() / parDef[i].readCoef;
                WEBprintln(temp);
                parDef[i].par->SetVal(temp);
            }
            return true;
        }
    }

    return false;
}

String param::GetWebString(const String &name)
{
    String retval = "";

    for (int i = 0; i < NUMBER_PAR; i++)
    {
        if (name == parDef[i].jmeno)
        {
            if (isnan(parDef[i].readCoef))
            {

                parDef[i].par->GetString(retval);
                retval.replace("\"", "");
            }
            else if (parDef[i].readCoef == 1)
            {
                retval = String(parDef[i].par->GetVal());
            }
            else if (parDef[i].readCoef == TIME_F)
            {
                parDef[i].par->GetString(retval);
                retval.replace("\"", "");
            }

            else
            {
                retval = String(parDef[i].par->GetVal() * parDef[i].readCoef);
            }
            break;
        }
    }
    return retval;
}

size_t param::allocEEPROM(size_t size)
{
    size_t adrr = EEPROM_SIZE;
    if ((lastEepAdr + size) < EEPROM_SIZE)
    {
        adrr = lastEepAdr;
        lastEepAdr += size;
    }
    return adrr;
}

void int32_pe::Init(int32_t def)
{

    if (!IsValid())
    {
        SetVal(def);
    }
}

void int32_pe::Init(void)
{
}
