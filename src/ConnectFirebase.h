#include <Arduino.h>
#include <FirebaseESP32.h>

#define DATABASE_URL "https://arduno-ir-default-rtdb.asia-southeast1.firebasedatabase.app/"


void initFirebase(void *xCreatedEventGroup);

void irRemote(void *xCreatedEventGroup);