#include <Arduino.h>
#include "ConnectWifi.h"
#include "ConnectFirebase.h"


SemaphoreHandle_t xBinarySemaphore;

void setup()
{
  pinMode(2, OUTPUT);
  FirebaseConfigObject firebaseConfigObject;
  Serial.begin(115200);
  xBinarySemaphore = xSemaphoreCreateBinary();
  firebaseConfigObject.xBinarySemaphore = &xBinarySemaphore;
  xTaskCreate(firebaseListener, "firebaseListener", 10000, &firebaseConfigObject, 1, NULL);
  xTaskCreate(initWifi, "initWifi", 10000, xBinarySemaphore, 3, NULL);
  xTaskCreate(initFirebase, "initFirebase", 10000, &firebaseConfigObject, 2, NULL);
  xSemaphoreGive(xBinarySemaphore);
}

void loop() {}
