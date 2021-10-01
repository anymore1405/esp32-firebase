#include "ConnectWifi.h"
#include "EventGroupConfig.h"


void initWifi(void *xCreatedEventGroup)
{
  while (1)
  {
    
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.print("Connecting to Wi-Fi");
      while (WiFi.status() != WL_CONNECTED)
      {
        Serial.print(".");
        delay(300);
      }
      Serial.println();
      Serial.print("Connected with IP: ");
      Serial.println(WiFi.localIP());
      Serial.println();
      xEventGroupSetBits(xCreatedEventGroup, BIT_INIT_FIREBASE);
      vTaskDelete(NULL);
  }
}
