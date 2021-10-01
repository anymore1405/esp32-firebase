#include <Arduino.h>
#include <FirebaseESP32.h>
// #include "addons/TokenHelper.h"
// #include "addons/RTDBHelper.h"

#define DATABASE_URL "https://arduno-ir-default-rtdb.asia-southeast1.firebasedatabase.app/"


void initFirebase(void *xBinarySemaphore);

void firebaseListener(void *xBinarySemaphore);

void irreverce(void *paramter);