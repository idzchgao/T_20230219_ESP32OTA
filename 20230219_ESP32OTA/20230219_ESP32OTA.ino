#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

const char * ssid = "JOEGHOME";
const char * password = "sll32401";

WiFiClient client;
HTTPClient https;

String FirmwareVer = "2.2";

#define URL_fw_Version "https://raw.githubusercontent.com/programmer131/ESP8266_ESP32_SelfUpdate/master/esp32_ota/bin_version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/programmer131/ESP8266_ESP32_SelfUpdate/master/esp32_ota/fw.bin"

//#define URL_fw_Version "http://cade-make.000webhostapp.com/version.txt"
//#define URL_fw_Bin "http://cade-make.000webhostapp.com/firmware.bin"

void connect_wifi();
void firmwareUpdate();
int FirmwareVersionCheck();

unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 10000;

void repeatedCall() {
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) 
  {
    previousMillis = currentMillis;
    if (FirmwareVersionCheck()) 
    {
      firmwareUpdate();
    }
  }
}

void setup() 
{
  Serial.begin(115200);
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
//  pinMode(LED_BUILTIN, OUTPUT);
  connect_wifi();
}
void loop() 
{
  repeatedCall();
}

void connect_wifi() {
  Serial.println("Waiting for WiFi");
  WiFi.disconnect(true);                   // 断开当前WIFI连接
  WiFi.mode(WIFI_STA);                     // 设置WIFI模式为STA模式
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void firmwareUpdate(void) {
//  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}
int FirmwareVersionCheck(void) {
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
//  WiFiClientSecure * client = new WiFiClientSecure;
  
  if (https.begin(fwurl)) 
  { // HTTPS      
    Serial.print("[HTTPS] GET...\n");
    // start connection and send HTTP header
    delay(100);
    httpCode = https.GET();
    delay(100);
    if (httpCode == HTTP_CODE_OK) // if version received
    {
      payload = https.getString(); // save received version
    } else {
      Serial.print("error in downloading version file:");
      Serial.println(httpCode);
    }
    https.end();
  }
      
  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    } 
    else 
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  } 
  return 0;  
}
