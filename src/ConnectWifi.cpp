#include "ConnectWifi.h"


void initWifi(void *xBinarySemaphore)
{
  while (1)
  {
    if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
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
      xSemaphoreGive(xBinarySemaphore);
      vTaskDelete(NULL);
    }
  }
}
