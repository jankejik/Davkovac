/*! \file Param.h
    \author Jan Kejík
*/

#include <Arduino.h>
#include "EEPROM.h"
#include "Common.h"

#ifndef __PARAM_H_
#define __PARAM_H_

#define EEPROM_SIZE 512

#ifdef WEB_PARAM_DEBUG

#define WEBprint(a) dbSerial.print(a)
#define WEBprintln(a) dbSerial.println(a)

#else

#define WEBprint(a)
#define WEBORPprintln(a)

#endif



typedef union
{
    uint64_t all;
    struct
    {
        int32_t val;
        int32_t inval;
    };

} Eepval_t;

class param
{
protected:
    bool change;
    param(void) : change(true) {}
    static size_t lastEepAdr;

public:
    static EEPROMClass eep;

    static String GetWebString(const String &hodnota);
    static bool PutWebString(const String &name, const String &hodnota);

    bool changed()
    {
        bool ch = change;
        change = false;
        return ch;
    }
    void SetChange(void)
    {
        change = true;
    }
    static size_t allocEEPROM(size_t size);

    virtual bool SetVal(int32_t newVal) { return false; }
    virtual bool SetVal(const String &newVal) { return false; }
    virtual bool SetVal(const char *newVal) { return false; }

    virtual int32_t GetVal(void) { return 0; }

    virtual size_t GetSize(void) { return 0; }

    virtual void GetString(String &retVal);

    virtual bool IsValid(void) { return true; }

    virtual bool GetText(char *text) { return false; }

    virtual bool GetData(uint8_t &dt) { return false; }
};

class int32_p : public param
{
protected:
    int32_t value;
    int32_t min;
    int32_t max;

public:
    int32_p(int32_t val, int32_t mi, int32_t ma)
    {
        value = val;
        min = mi;
        max = ma;
        change = true;
    }
    int32_p(void)
    {
        value = 0;
        min = INT32_MIN;
        max = INT32_MAX;
        change = true;
    }
    bool SetVal(float newVal)
    {
        int32_t temp = newVal;
        return SetVal(temp);
    }
    bool SetVal(int32_t newVal)
    {
        if (newVal > max)
        {
            newVal = max;
            change = true;
        }
        else if (newVal < min)
        {
            newVal = min;
            change = true;
        }

        if (newVal != value)
        {
            change = true;
            value = newVal;
        }
        return true;
    }
    int32_t GetVal(void)
    {
        return value;
    }

    void GetString(String &retVal)
    {
        retVal = value;
        change = false;
    }

    bool IsValid(void)
    {

        int32_t temp = GetVal();
        return temp >= min && temp <= max;
    }

    int32_p &operator=(int32_t &in)
    {
        SetVal(in);
        return *this;
    }

    operator int32_t() const { return value; }

    bool operator==(const int32_t &in) { return value == in; }
    bool operator<=(const int32_t &in) { return value <= in; }
    bool operator>=(const int32_t &in) { return value >= in; }
    bool operator<(const int32_t &in) { return value < in; }
    bool operator>(const int32_t &in) { return value > in; }

    bool operator==(int in) { return value == in; }
    bool operator<=(int in) { return value <= in; }
    bool operator>=(int in) { return value >= in; }
    bool operator<(int in) { return value < in; }
    bool operator>(int in) { return value > in; }
};

class int32_pe : public param
{

protected:
    size_t adr;
    int32_t min;
    int32_t max;

public:
    int32_pe(int32_t mi, int32_t ma)
    {
        min = mi;
        max = ma;
        change = true;
        adr = allocEEPROM(GetSize());
    }
    void Init(int32_t def);
    void Init(void);

    void Max(int32_t ma)
    {
        max = ma;
    }

    void Min(int32_t mi)
    {
        min = mi;
    }
    bool SetVal(int32_t newVal)
    {
        if (newVal > max)
        {
            newVal = max;
            change = true;
        }
        else if (newVal < min)
        {
            newVal = min;
            change = true;
        }
        Eepval_t temp;
        temp.all = eep.readULong64(adr);
        if (newVal != temp.val)
        {
            dbSerialPrint("EEprom set: ");
            dbSerialPrint(adr);
            dbSerialPrint(": = ");
            dbSerialPrintln(newVal);
            change = true;
            temp.val = newVal;
            temp.inval = -newVal;
            eep.writeULong64(adr, temp.all);
            eep.commit();
        }
        return true;
    }

    int32_t GetVal(void)
    {
        Eepval_t temp;
        temp.all = eep.readULong64(adr);
        return temp.val;
    }
    size_t GetSize()
    {
        return sizeof(Eepval_t);
    }

    void GetString(String &retVal)
    {
        int32_t value = GetVal();
        retVal = value;
        // dbSerialPrint("EEprom get: ");
        // dbSerialPrintln(value);
        change = false;
    }

    bool IsValid(void)
    {
        Eepval_t temp;
        temp.all = eep.readULong64(adr);
        dbSerialPrint("IsValid: ");
        dbSerialPrintln(temp.val);
        dbSerialPrintln(temp.inval);
        return (temp.val == (-temp.inval)) && (temp.val >= min) && (temp.val <= max);
    }

    int32_pe &operator=(int32_t &in)
    {
        SetVal(in);
        return *this;
    }
    operator int32_t() { return GetVal(); }

    bool operator==(const int32_t &in) { return GetVal() == in; }
    bool operator<=(const int32_t &in) { return GetVal() <= in; }
    bool operator>=(const int32_t &in) { return GetVal() >= in; }
    bool operator<(const int32_t &in) { return GetVal() < in; }
    bool operator>(const int32_t &in) { return GetVal() > in; }

    bool operator==(int in) { return GetVal() == in; }
    bool operator<=(int in) { return GetVal() <= in; }
    bool operator>=(int in) { return GetVal() >= in; }
    bool operator<(int in) { return GetVal() < in; }
    bool operator>(int in) { return GetVal() > in; }
};

class time_pe : public int32_pe
{

public:
    time_pe(void) : int32_pe(0, HOUR_PER_DAY * MIN_PER_HOUR * SEC_PER_MIN-SEC_PER_MIN) {}

    void GetString(String &retVal)
    {
        uint16_t th;
        uint16_t tm;
        int32_t value = GetVal();
        th = value / (MIN_PER_HOUR * SEC_PER_MIN);
        tm = value % (MIN_PER_HOUR * SEC_PER_MIN);
        tm = tm / SEC_PER_MIN;
        String temp = "\"";
        if(th < 10)
        {
            temp += '0';
        }    
        temp += th;
        temp += ':';
        if(tm < 10)
        {
            temp += '0';
        }
        temp += tm;
        temp += "\"";
        retVal = temp;
        Serial.println(th);
        Serial.println(tm);
        Serial.println(value);
        change = false;
    }
};

class String_p : public param
{
private:
    String text;

public:
    String_p(String tex)
    {
        text = tex;
        change = true;
    }
    String_p(size_t length)
    {
        text.reserve(length);
        text = "";
        change = true;
    }
    bool SetVal(const String &newVal)
    {
        if (text.equals(newVal))
        {
            return false;
        }
        text = newVal;
        change = true;
        return true;
    }

    bool SetVal(const char *txt)
    {
        text = txt;
        change = true;
        return true;
    }

    size_t GetSize(void)
    {
        return text.length() + 1;
    }

    void GetString(String &retVal)
    {
        dbSerialPrintln(text);
        retVal = "\"";
        retVal += text;
        retVal += "\"";
        change = false;
    }

    bool GetVal(String &retVal)
    {
        retVal = text;
        return true;
    }

    String_p &operator=(const char *in)
    {
        SetVal(in);
        return *this;
    }
    String_p &operator=(String in)
    {
        SetVal(in);
        return *this;
    }

    operator String() const { return text; }

    String_p &operator=(String_p in)
    {
        in.GetVal(text);
        change = true;
        return *this;
    }
};

class String_pe : public param
{
private:
    const size_t maxLen;
    size_t adr;

public:
    String_pe(size_t len) : maxLen(len)
    {
        change = true;
        adr = allocEEPROM(GetSize());
    }
    bool SetVal(const String &newVal)
    {

        if (newVal.length() < maxLen)
        {
            char txt[maxLen];
            eep.readBytes(adr, txt, maxLen);
            if (newVal.equals(txt))
            {
                return false;
            }
            eep.writeBytes(adr, newVal.c_str(), maxLen);
            eep.commit();
            change = true;

            return true;
        }

        return false;
    }

    bool SetVal(const char *newVal)
    {

        String str = newVal;

        if (str.length() < maxLen)
        {
            char txt[maxLen];
            eep.readBytes(adr, txt, maxLen);
            if (str.equals(txt))
            {
                return false;
            }
            eep.writeBytes(adr, newVal, maxLen);
            eep.commit();
            change = true;
            return true;
        }

        return false;
    }
    bool GetVal(String &retVal)
    {
        char txt[maxLen];
        eep.readBytes(adr, txt, maxLen);
        retVal = txt;
        return true;
    }
    bool GetText(char *text)
    {
        eep.readBytes(adr, text, maxLen);
        return true;
    }
    size_t GetSize()
    {
        return maxLen + 1;
    }
    void GetString(String &retVal)
    {
        dbSerialPrintln("->GetString");
        char txt[maxLen];
        eep.readBytes(adr, txt, maxLen);

        retVal = "\"";
        retVal += txt;
        retVal += "\"";
        dbSerialPrintln(retVal);
        dbSerialPrintln(txt);
        change = false;
    }

    String_pe &operator=(const char *in)
    {
        SetVal(in);
        return *this;
    }
    String_pe &operator=(String in)
    {
        SetVal(in);
        return *this;
    }
};

class waveform : public param
{
protected:
    size_t maxSamples;
    size_t nmrSamples;
    size_t headIdx;
    size_t tailIdx;
    uint8_t *valBuf;
    size_t newSamples;
    uint8_t maxVal;

public:
    waveform(size_t samples = 255, uint8_t maxVa = 255)
    {
        valBuf = new uint8_t[samples];
        maxSamples = samples;
        nmrSamples = 0;
        headIdx = 0;
        tailIdx = 0;
        newSamples = 0;
        change = true;
        maxVal = maxVa;
        memset(valBuf, 0, samples);
    }

    bool pop(uint8_t &val)
    {
        if (tailIdx != headIdx)
        {
            val = valBuf[tailIdx];
            if (tailIdx == (maxSamples - 1))
            {
                tailIdx = 0;
            }
            else
            {
                tailIdx++;
            }
            return true;
        }
        else
        {
            //   val = 0;
        }

        return false;
    }
    bool push(uint8_t val)
    {
        bool retval = false;

        valBuf[headIdx] = val;
        headIdx++;
        retval = true;

        if (headIdx >= maxSamples)
        {
            headIdx = 0;
            if (tailIdx == 0)
            {
                tailIdx++;
            }
        }
        if (tailIdx == headIdx)
        {
            if (tailIdx == (maxSamples - 1))
            {
                tailIdx = 0;
            }
            else
            {
                tailIdx++;
            }
        }

        return retval;
    }
    bool SetVal(int32_t newVal)
    {
        if (newVal > maxVal)
        {
            newVal = maxVal;
        }
        else if (newVal < 0)
        {
            newVal = 0;
        }

        bool retval = push((uint8_t)newVal);
        if (retval)
        {
            if (newSamples < maxSamples)
            {
                newSamples++;
            }
            change = true;
        }
        return retval;
    }

    void GetString(String &retVal)
    {
        retVal = newSamples;
        change = false;
    }

    bool GetData(uint8_t &dt)
    {
        bool retval = false;
        if (newSamples > 0)
        {
            pop(dt);
            Serial.println(dt);
            newSamples--;
            retval = true;
        }
        return retval;
    }
    size_t GetSize(void)
    {
        return newSamples;
    }
    String WebSamples(void)
    {
        size_t index = headIdx;
        String temp = "";
        for (int i = 0; i < maxSamples; i++)
        {
            if (index == 0)
            {
                index = maxSamples - 1;
            }
            else
            {
                index--;
            }
            temp += valBuf[index];
            if (i != maxSamples - 1)
            {
                temp += ";";
            }
        }
        return temp;
    }
};

#endif /*__PARAM_H_*/
