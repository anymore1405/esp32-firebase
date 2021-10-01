#include <Arduino.h>
#include "ConnectWifi.h"
#include "ConnectFirebase.h"

QueueHandle_t xQueue;

void setup()
{
  pinMode(2, OUTPUT);
  FirebaseConfigObject firebaseConfigObject(xSemaphoreCreateBinary());
  
  xQueue = xQueueCreate(1, sizeof(FirebaseConfigObject *));
  while (xQueue == NULL)
  {
    Serial.println("Re-Create xQueue");
  }

  xQueueSend(xQueue, &firebaseConfigObject, 0);

  Serial.begin(115200);
  
  xTaskCreate(firebaseListener, "firebaseListener", 10000, xQueue, 1, NULL);
  xTaskCreate(initWifi, "initWifi", 10000, firebaseConfigObject.xBinarySemaphore, 3, NULL);
  xTaskCreate(initFirebase, "initFirebase", 10000, xQueue, 2, NULL);
  xSemaphoreGive(firebaseConfigObject.xBinarySemaphore);
}

void loop() {}
