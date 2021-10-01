#include <Arduino.h>
#include "ConnectWifi.h"
#include "ConnectFirebase.h"

QueueHandle_t xQueue;

void setup()
{
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  FirebaseConfigObject firebaseConfigObject;
  firebaseConfigObject.xBinarySemaphore = xSemaphoreCreateBinary();
  Serial.println("created firebaseConfigObject");
  xQueue = xQueueCreate(1, sizeof(FirebaseConfigObject*));
  while (xQueue == NULL)
  {
    Serial.println("Re-Create xQueue");
    xQueue = xQueueCreate(1, sizeof(FirebaseConfigObject*));
  }
  Serial.println("created queue");

  Serial.println("send queue");
  
  xSemaphoreGive(firebaseConfigObject.xBinarySemaphore);
  
  xTaskCreate(initFirebase, "initFirebase", 1000, xQueue, 2, NULL);
  Serial.println("initFirebase");
  xTaskCreate(initWifi, "initWifi", 1000, firebaseConfigObject.xBinarySemaphore, 3, NULL);
  Serial.println("initWifi");
  xTaskCreate(firebaseListener, "firebaseListener", 1000, xQueue, 1, NULL);
  Serial.println("firebaseListener");
  Serial.println("xSemaphoreGive");
  delay(2000);
  xQueueSend(xQueue, &firebaseConfigObject, 0);
}

void loop() {}
