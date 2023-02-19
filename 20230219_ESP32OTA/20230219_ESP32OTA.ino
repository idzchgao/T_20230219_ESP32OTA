#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>

const char * ssid = "JOEGHOME";
const char * password = "sll32401";

WiFiClient client;
HTTPClient https;

String FirmwareVer = "2.3";

#define URL_fw_Version "https://raw.githubusercontent.com/idzchgao/T_20230219_ESP32OTA/main/ver.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/idzchgao/T_20230219_ESP32OTA/main/fw.bin"

//#define URL_fw_Version "http://cade-make.000webhostapp.com/version.txt"
//#define URL_fw_Bin "http://cade-make.000webhostapp.com/firmware.bin"

void connect_wifi();
void firmwareUpdate();
int FirmwareVersionCheck();

unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 60000;

void repeatedCall() {
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) 
  {
    previousMillis = currentMillis;
    
    if(FirmwareVersionCheck())    //if (FirmwareVersionCheck()) 
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

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void firmwareUpdate(void) {
//  httpUpdate.setLedPin(LED_BUILTIN, LOW);
  if ((WiFi.status() == WL_CONNECTED))
  {    
//    WiFiClient client;
    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    // httpUpdate.setLedPin(LED_BUILTIN, LOW);
//
//    ESPhttpUpdate.onStart(update_started);
//    ESPhttpUpdate.onEnd(update_finished);
//    ESPhttpUpdate.onProgress(update_progress);
//    ESPhttpUpdate.onError(update_error);
  
    t_httpUpdate_return ret = ESPhttpUpdate.update(URL_fw_Bin);
  
    switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
  
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;
  
    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
    }    
    
  }

}
int FirmwareVersionCheck(void) {
  String payload;
  int httpCode;
//  String fwurl = "";
//  fwurl += URL_fw_Version;
//  fwurl += "?";
//  fwurl += String(rand());
//  Serial.println(fwurl);
//  WiFiClientSecure * client = new WiFiClientSecure;
  
  if (https.begin(URL_fw_Version)) 
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
      Serial.printf("\r\nDevice already on latest firmware version:%s\r\n", FirmwareVer);
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
