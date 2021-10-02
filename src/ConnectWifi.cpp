#include "ConnectWifi.h"
#include "EventGroupConfig.h"

void initWifi(void *xCreatedEventGroup)
{
  xEventGroupSetBits(xCreatedEventGroup, BIT_LED_INIT_WIFI);
  while (1)
  {
    xEventGroupClearBits(xCreatedEventGroup, BIT_LED_START);
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
    xEventGroupClearBits(xCreatedEventGroup, BIT_LED_INIT_WIFI);
    xEventGroupSetBits(xCreatedEventGroup, BIT_INIT_FIREBASE);
    xEventGroupSetBits(xCreatedEventGroup, BIT_LED_INIT_FIREBASE);
    vTaskDelete(NULL);
  }
}
