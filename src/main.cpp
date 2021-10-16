#include <Arduino.h>
#include "ConnectFirebase.h"
#include "ConnectLedSignal.h"
#include "EventGroupConfig.h"
// #include "ConfigConnection.h"
// #define configUSE_16_BIT_TICKS 0
EventGroupHandle_t xCreatedEventGroup;
#include <WiFi.h>

void setup()
{
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true,true);
  Serial.begin(115200);
  xCreatedEventGroup = xEventGroupCreate();
  if(xCreatedEventGroup == NULL){
    Serial.println("xCreatedEventGroup failed");
  }
  xEventGroupSetBits(xCreatedEventGroup, BIT_LED_INIT_WIFI);
  xEventGroupSetBits(xCreatedEventGroup, BIT_INIT_WIFI);

  xTaskCreate(ledSignal, "ledSignal", 700, xCreatedEventGroup, 3, NULL);

  xTaskCreate(initFirebase, "initFirebase", 10000, xCreatedEventGroup, 2, NULL);

  xTaskCreate(irRemote, "irRemote", 10000, xCreatedEventGroup, 2, NULL);

  // xTaskCreate(configConnection, "configConnection", 10000, xCreatedEventGroup, 2, NULL);

}

void loop() {}
