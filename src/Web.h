/*! \file Web.h
    \author Jan Kejík
*/

#include "Arduino.h"

#include "SPIFFS.h"
#include "Update.h"
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESPmDNS.h>
#include "ArduinoJson.h"
#include "TimeLib.h"
#include "Param.h"
#include "Common.h"

#define TIME_LOAD_PERIOD 5000
#define WIFI_LOAD_PERIOD 2000

#ifndef __WEB_H_
#define __WEB_H_


extern void WebInit(void);

extern bool WifiMode(void);
extern void WifiReconnect(void);

#endif /*__WEB_H_*/