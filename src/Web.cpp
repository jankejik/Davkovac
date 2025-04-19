#include "Web.h"
#include "ChangeIcon.h"
#include "TempReading.h"
#include "PHReading.h"
#include "ORPReading.h"
#include "TimeLib.h"
#include "NTPClient.h"
#include "InfoMsg.h"

#define HOSTNAME "Davkovac"

String_pe SSID(50);
String_pe displaySSID(50);
String_pe Password(50);
int32_pe WifiAP(0, 1);
String_p OurTime("--:--");

extern waveform phForm;
extern waveform tempForm;
extern waveform orpForm;

extern waveform phForm1;
extern waveform tempForm1;
extern waveform orpForm1;

extern waveform phForm24;
extern waveform tempForm24;
extern waveform orpForm24;

extern String_p message[];

extern Temp temp_01C;

extern PHReading pH_01;
extern ORPreading orp;
extern int32_pe flocSetPoint;

extern int32_pe phAutoCtrlU;
extern int32_pe phAutoCtrlD;
extern int32_pe orpAutoCtrl;
extern int32_pe flocAutoCtrl;

#define RESTART_COUNTER 2000

ChangeIcon WifiIcon(42, 40, 41);

// *************  WiFi
IPAddress ap_local_IP(192, 168, 1, 1);
IPAddress ap_gateway(192, 168, 1, 1);
IPAddress ap_subnet(255, 255, 255, 0);

String ap_hostname = "Davkovac";
bool WiFi_On(bool AP); // true - Access Point, false - client
String WiFi_GetIP(void);
String New_WiFi_name = "";
void WiFiEvent(WiFiEvent_t event);
bool WiFi_disconnected = false;

// *************  TIME
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String GetDateStr(void);
String GetTimeStr(void);

void BuildTime(void)
{
  String hodina = String(hour());
  if (hodina.length() == 1)
    hodina = "0" + hodina;
  String minuta = String(minute());
  if (minuta.length() == 1)
    minuta = "0" + minuta;
  OurTime.SetVal(hodina + ":" + minuta);
}

// ************* WEB Server
DynamicJsonDocument ini(4096);

AsyncWebServer server(80);
String New_password_web = "";
String vyber;

String ActStatus(void)
{
  String temp;
  String val;
  temp_01C.GetString(temp);
  val = temp;
  pH_01.GetString(temp);
  val += ";";
  val += temp;
  orp.GetString(temp);
  val += ";";
  val += temp;
  flocSetPoint.GetString(temp);
  val += ";";
  val += temp;

  return val;
}

String ActReg(void)
{
  String temp;
  String val;
  phAutoCtrlU.GetString(temp);
  val = temp;
  phAutoCtrlD.GetString(temp);
  val += ";";
  val += temp;
  orpAutoCtrl.GetString(temp);
  val += ";";
  val += temp;
  flocAutoCtrl.GetString(temp);
  val += ";";
  val += temp;

  return val;
}

// ************* Update firmware
void handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
size_t total = 1;
size_t actual = 0;
int cmd = U_SPIFFS;

const char recovery_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><head><meta charset='UTF-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Obnova softwaru</title></head>
<body><form method='POST' action='/update' enctype='multipart/form-data'><H1>Obnova softwaru</H1><P>Nahrát firmware/souborový systém</P><input type='file' name="filename" accept='.bin'>
<P>Nahrát software (bude to trvat přibližně 1 minutu)</P><input type='submit' value='Nahrát'></form></body></html>)rawliteral";


// *************   Firmware Update **************************************************************************************************

void handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    actual = 0;
    total = request->contentLength();
    cmd = filename.endsWith("spiffs.bin") ? U_SPIFFS : U_FLASH;
    Serial.println(cmd);
    if (cmd == U_FLASH && !filename.endsWith("firmware.bin"))
    {
      request->send(404);
      return;
    }
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
    {
      Update.printError(Serial);
      request->send(404);
    }
  }
  if (Update.write(data, len) != len)
  {
    request->send(404);
  }
  else
  {
    actual += len;
  }
  if (final)
  {
    if (!Update.end(true))
    {
      Update.printError(Serial);
      request->send(404, "text/plain", "Pri nahravani doslo k chybe! Nyni dojde k restartu zarizeni");
      delay(1000);
      ESP.restart();
    }
    else
    {
      Serial.println("UPDATE PROBEHL USPESNE");
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Update probehl uspesne, nyni dojde k restartu zarizeni.");
      response->addHeader("Refresh","20; url=/zakladni.html");
      response->addHeader("Connection","close");
      request->send(response);
      //  if (cmd == U_FLASH)
      //  {
      delay(1000);
      ESP.restart();
      //  }
    }
  }
}

void Log(String hlaseni)
{
  dbSerialPrintln(hlaseni);
}
void RES_printProgress(size_t prg, size_t sz)
{
  dbSerialPrintln(prg);
  dbSerialPrintln(sz);
}

// *************  WEB Server  ************************************************************************************************************

String WEB_processor(const String &varib)
{
  return param::GetWebString(varib);
}

// *************  WIFI mode  ************************************************************************************************************

bool WifiMode(void)
{
  bool retval = (bool)WifiAP.GetVal();
  static enum {
    not_Passed,
    AP_active,
    AP_off,
  } AP_status = not_Passed;

  if (AP_status == not_Passed)
  {
    AP_status = (retval) ? AP_active : AP_off;
  }
  else if (retval)
  {
    if (AP_status == AP_off)
    {
      delay(1000);
      ESP.restart();
    }
  }
  else
  {
    if (AP_status == AP_active)
    {
      delay(1000);
      ESP.restart();
    }
  }

  return retval;
}

void WifiReconnect(void)
{
  bool retval = (bool)WifiAP.GetVal();
  if (!retval)
  {
    if (WiFi_disconnected)
    {
      server.end();
      WebInit();
    }
  }
}

void WebInit(void)
{
  WifiAP.Init(1);
  //WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.onEvent(WiFiEvent);
  // inicializujeme souborový systém
  if (!SPIFFS.begin(true))
  {
    Log("ERROR has occured while mounting SPIFS");
    return;
  }
  else
  {
    Log("SPIFS system successfully mounted");
  }
  if (WifiMode())
  {
    // WiFi AP
    WiFi_On(true);
  }
  else
  {
    // WiFi Client
    if (!WiFi_On(false))
    {
      // WiFi_On(true);
      return ;
    } // pokud se nepodaří připojit k WiFi, spustíme AP a mode CONFIG
  }

  // Připojíme www stránky
  server.serveStatic("/www/", SPIFFS, "/").setCacheControl("max-age=600");
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404); });
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->redirect("/zakladni.html"); });

  server.on("/aktualizace.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/aktualizace.html", String(), false); });

  server.on("/recovery", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", recovery_html); });            

  server.on("/pripojeni.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    int paramsNr = request->params();
    String hostname;
    String pass;
    if (paramsNr == 3)
    {
      AsyncWebParameter *h = request->getParam(1);
      AsyncWebParameter *p = request->getParam(2);
      hostname = h->value();
      pass = p->value();

      if (hostname != "")
      {
        SSID.SetVal(hostname);
        Password.SetVal(pass);
        request->send(200, "text/plain", "Dojde k restartu zarizeni!!");
        WifiAP.SetVal(0);
        delay(1000);
        ESP.restart();
      }
    }
      request->send(SPIFFS, "/pripojeni.html", String(), false, WEB_processor); });

  server.on("/progress", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(actual * 100 / total)); });

  server.on("/statusData", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", ActStatus()); });

  server.on("/getautostate", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", ActReg()); });

  server.on("/chartpH", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    int paramsNr = request->params();
    if (paramsNr == 1)
    {
      AsyncWebParameter *p = request->getParam(0);
      if(p->value() == "1")
      {
        request->send(200, "text/plain", phForm1.WebSamples()); 
      }
      else if(p->value() == "4")
      {
        request->send(200, "text/plain", phForm.WebSamples());  
      }
         else if(p->value() == "24")
      {
        request->send(200, "text/plain", phForm24.WebSamples());  
      }
        } });

  server.on("/chartORP", HTTP_GET, [](AsyncWebServerRequest *request)
            {
               int paramsNr = request->params();
    if (paramsNr == 1)
    {
      AsyncWebParameter *p = request->getParam(0);
      if(p->value() == "1")
      {
        request->send(200, "text/plain", orpForm1.WebSamples()); 
      }
      else if(p->value() == "4")
      {
        request->send(200, "text/plain", orpForm.WebSamples());  
      }
         else if(p->value() == "24")
      {
        request->send(200, "text/plain", orpForm24.WebSamples());  
      }
     }
          });

  server.on("/chartTemp", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              
              int paramsNr = request->params();
    if (paramsNr == 1)
    {
      AsyncWebParameter *p = request->getParam(0);
      if(p->value() == "1")
      {
        request->send(200, "text/plain", tempForm1.WebSamples()); 
      }
      else if(p->value() == "4")
      {
        request->send(200, "text/plain", tempForm.WebSamples());  
      }
         else if(p->value() == "24")
      {
        request->send(200, "text/plain", tempForm24.WebSamples());  
      }
     }
             });

  /*server.on("/timeAct", HTTP_GET, [](AsyncWebServerRequest *request)
            { char buff[50];
              ltoa(timeClient.getEpochTime(), buff, 10);
               request->send(200, "text/plain", buff); });
*/
  server.on("/automatic", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              int paramsNr = request->params();
              if(paramsNr == 1){
              AsyncWebParameter* p = request->getParam(0);
              param::PutWebString(p->name(), p->value());
     
  }    
  request->send(200); });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", "Davkovac"); });
  server.on(
      "/update", HTTP_POST,
      [](AsyncWebServerRequest *request) {
      },
      [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
      { handleDoUpdate(request, filename, index, data, len, final); });
  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request)
            {

      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Please wait while device reboots...");
      response->addHeader("Refresh","20; url=/zakladni.html");
      response->addHeader("Connection","close");
      request->send(response);
      delay(1000);
      ESP.restart();
    });

  server.on("/zakladni.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/zakladni.html", String(), false, WEB_processor); });

  server.on("/nastaveni.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  int paramsNr = request->params();
  if(paramsNr == 2){
    AsyncWebParameter* p = request->getParam(1);
    // Serial.print("Hodnota P:");
    //          Serial.println(p->value());
     param::PutWebString(p->name(), p->value());
     
  } 
  if(paramsNr > 2)
  {
    AsyncWebParameter* p = request->getParam(2);
    param::PutWebString(p->name(), p->value());
    if(paramsNr > 3)
    { 
        AsyncWebParameter* b = request->getParam(3);
        param::PutWebString(b->name(), b->value());   
    }
  } 
   
  request->send(SPIFFS, "/nastaveni.html", String(), false, WEB_processor); });
  server.serveStatic("/", SPIFFS, "/");

  // spustíme web server
  server.begin();
  //Update.onProgress(RES_printProgress);
  // MDNS.addService("http", "tcp", 80);
  if (!WifiAP)
  {
    Msg("Pripojeno k  '%s' ", WiFi.SSID().c_str());
    WebLog("Wifi", "Připojeno k  %s ", WiFi.SSID().c_str());
    displaySSID.SetVal(WiFi.SSID());
    Msg("IP: %s", WiFi_GetIP().c_str());
    WebLog("Wifi", "IP: %s", WiFi_GetIP().c_str());
  }
  else
  {
    Msg("AP: '%s.local'", HOSTNAME);
    WebLog("Wifi", " Připojený k AP: %s.local", HOSTNAME);
    displaySSID.SetVal("AP");
    Msg("IP: %s", WiFi_GetIP().c_str());
    WebLog("Wifi", "IP: %s", WiFi_GetIP().c_str());
  }

  Log("");
  Log("*** LOOP ***");
  Log("");
}
// *************   WiFi  ************************************************************************************************************
;
void WiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_WIFI_READY:

    Log(F("WiFi interface ready"));
    break;
  case SYSTEM_EVENT_STA_START:
    Log(F("WiFi client started"));

    break;
  case SYSTEM_EVENT_STA_STOP:
    Log(F("WiFi clients stopped"));
    WifiIcon.SetVal(0);
    break;
  case SYSTEM_EVENT_STA_CONNECTED:
    Log(F("WiFi Connected to access point"));
        WiFi_disconnected = false;

    WifiIcon.SetVal(2);
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Log(F("WiFi Disconnected from WiFi access point. Try again to connect after 30s ..."));
    WiFi.disconnect(true);
    WifiIcon.SetVal(0);
    WiFi.mode(WIFI_MODE_NULL);
    WiFi_disconnected = true;
    break;
  case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    Log(F("WiFi Authentication mode of access point has changed"));
    break;
  case SYSTEM_EVENT_STA_GOT_IP:
    Log("WiFi Obtained IP address: " + WiFi_GetIP());
    break;
  case SYSTEM_EVENT_STA_LOST_IP:
    Log(F("WiFi Lost IP address and IP address is reset to 0"));
    WifiIcon.SetVal(0);
    break;

  default:
    break;
  }
  WiFi.removeEvent(event);
}

String WiFi_GetIP(void)
{
  IPAddress ips;

  if (WifiAP.GetVal() == 1)
  {
    ips = WiFi.softAPIP();
  }
  else
  {
    ips = WiFi.localIP();
  }

  char _IP[20];
  sprintf(_IP, "%d.%d.%d.%d", ips[0], ips[1], ips[2], ips[3]);
  return String(_IP);
}

bool WiFi_On(bool AP)
{
  int i = 0;
  char text_pole[200];
  IPAddress ip;
  if (AP)
  {

    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_AP);

    Log(WiFi.softAP(ap_hostname.c_str()) ? "WiFi AP setup" : "ERROR WiFi AP setup");
    delay(100);
    bool apActive = WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet);
    Log(apActive ? "WiFi AP Started" : "Error WiFi AP not Start");
    if (apActive)
    {
      WifiIcon.SetVal(1);
    }
    else
    {
      WifiIcon.SetVal(0);
    }

    WiFi.setHostname(HOSTNAME);
    if (MDNS.begin(HOSTNAME))
    {
      sprintf(text_pole, "AP: '%s.local'IP: %s", HOSTNAME, WiFi_GetIP().c_str());

      Log(String(text_pole));
      MDNS.addService("http", "tcp", 80);
    }
    else
    {
      sprintf(text_pole, "Chyby DNS pro: \"%s.local\"", HOSTNAME);
      Err(text_pole);
      Log(String(text_pole));
    }
  }
  else
  {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(HOSTNAME);

    char ssidtemp[50];
    char passtemp[50];
    SSID.GetText(ssidtemp);
    Password.GetText(passtemp);

    WiFi.begin(ssidtemp, passtemp);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      i++;
      if (i > 10)
        break;
    }
    if (WiFi.status() != WL_CONNECTED)
    {

      WiFi.mode(WIFI_OFF);
      WiFi_disconnected = true;
      Msg("Nepodarilo se pripojit k Wifi!");

      return false;
    }
    else
    {
     WiFi_disconnected = false;

    }

    // Initialize mDNS
    if (!MDNS.begin(HOSTNAME))
    {
      sprintf(text_pole, "mDNS ERROR responder not started on domain: %s.local", HOSTNAME);
      Log(String(text_pole));
      return false;
    }

    MDNS.addService("http", "tcp", 80);
    timeClient.begin();
    timeClient.setTimeOffset(2 * 3600);
    timeClient.setUpdateInterval(60000); // aktualizujeme každou minutu
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
      delay(200);
      Serial.print(".");
      i++;
      if (i > 10)
        break;
    }
    if (i <= 10)
    {
      Log(F("NTP successfully started"));
      setTime(timeClient.getEpochTime());
    }
    else
    {
      Log(F("NTP ERROR Server on this site not found, GSM time enabled"));
    }
  }
  return true;
}
