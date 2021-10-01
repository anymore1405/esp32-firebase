#include <Arduino.h>
#include "ConnectWifi.h"
#include "ConnectFirebase.h"
SemaphoreHandle_t xBinarySemaphore;
void setup()
{
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  xBinarySemaphore = xSemaphoreCreateBinary();  
  xSemaphoreGive(xBinarySemaphore);
  
  xTaskCreate(initWifi, "initWifi", 10000, xBinarySemaphore, 3, NULL);
  Serial.println("initWifi");
  xTaskCreate(initFirebase, "initFirebase", 10000, xBinarySemaphore, 2, NULL);
  Serial.println("initFirebase");
  xTaskCreate(firebaseListener, "firebaseListener", 10000, xBinarySemaphore, 1, NULL);

}

void loop() {}
