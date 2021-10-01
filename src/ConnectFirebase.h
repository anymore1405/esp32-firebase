#include <Arduino.h>
#include <FirebaseESP32.h>
// #include "addons/TokenHelper.h"
// #include "addons/RTDBHelper.h"

#define DATABASE_URL "https://arduno-ir-default-rtdb.asia-southeast1.firebasedatabase.app/"

class FirebaseConfigObject {
    public:
        FirebaseData fbdo;
        FirebaseData stream;

        String path = "/test/int";

        FirebaseAuth auth;
        FirebaseConfig config;
        SemaphoreHandle_t xBinarySemaphore;
};

void initFirebase(void *xQueue);

void firebaseListener(void *xQueue);