#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "ConnectFirebase.h"
FirebaseData fbdo;
FirebaseData stream;

String path = "/test/int";

FirebaseAuth auth;
FirebaseConfig config;
FirebaseJsonData f;

const uint16_t kRecvPin = 14;
const uint16_t kIrLedPin = 4;
const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;
const uint16_t kFrequency = 38000;
uint16_t length;

IRsend irsend(kIrLedPin);
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, false);
decode_results results;

void initFirebase(void *xBinarySemaphore)
{
  for (;;)
  {
    if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
    {
      Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

      config.api_key = "AIzaSyBuwoPc2k1LLwb0Rg0CQ5R3B-kU2G99Tz4";

      //Assign the user sign in credentials
      auth.user.email = "khactai14052000@gmail.com";
      auth.user.password = "123456";
      config.database_url = DATABASE_URL;
      //   firebaseConfigObject.config.token_status_callback = tokenStatusCallback;

      //Initialize the library with the Firebase authen and config.
      Firebase.begin(&config, &auth);

      // Firebase.FCM.setServerKey("AAAAuZ7LBGE:APA91bE8i223J1XUU0snbgjbisVdoq1VmMeHiNHztIKjF9TSgFcf1QX8VAcy-WaSV8AB6eobxLxca6AMUEkCShForLjft49tfLa-p4ZdifEcWAhDTnvV9V3oaZxLC2VykNFOMBkpv7ax");
      //Optional, set AP reconnection in setup()
      Firebase.reconnectWiFi(true);

      if (!Firebase.RTDB.beginStream(&stream, "/test/int"))
        Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
      //Optional, set number of error retry
      // Firebase.RTDB.setMaxRetry(&fbdo, 3);

      // //Optional, set number of error resumable queues
      // Firebase.RTDB.setMaxErrorQueue(&fbdo, 30);

      // //Optional, use classic HTTP GET and POST requests.
      // //This option allows get and delete functions (PUT and DELETE HTTP requests) works for
      // //device connected behind the Firewall that allows only GET and POST requests.
      // Firebase.RTDB.enableClassicRequest(&fbdo, true);

      // //Optional, set the size of HTTP response buffer
      // //Prevent out of memory for large payload but data may be truncated and can't determine its type.
      // fbdo.setResponseSize(1024); //minimum size is 1024 bytes
      xSemaphoreGive(xBinarySemaphore);
      vTaskDelete(NULL);
    }
  }
}

void firebaseListener(void *xBinarySemaphore)
{
  irsend.begin();
  for (;;)
  {
    if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdTRUE)
    {

      if (Firebase.ready())
      {
        if (!Firebase.RTDB.readStream(&stream))
          Serial.printf("sream read error, %s\n\n", stream.errorReason().c_str());

        if (stream.streamTimeout())
          Serial.println("stream timeout, resuming...\n");

        if (stream.streamAvailable())
        {

          Serial.printf("sream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\nvalue, %d\n\n",
                        stream.streamPath().c_str(),
                        stream.dataPath().c_str(),
                        stream.dataType().c_str(),
                        stream.eventType().c_str(),
                        stream.intData());

          if (stream.dataType() == "int")
          {
            if (stream.intData() == 1)
            {
              digitalWrite(2, HIGH);
              Firebase.getArray(fbdo, "array/test");
              Serial.println("Firebase.getArray");
              Serial.println("Firebase.arrPtr");
              fbdo.jsonArrayPtr()->get(f, 0);
              Serial.println("arrPtr->get(f, 0)");
              length = f.to<uint16_t>();
              Serial.println("to uint");
              uint16_t raw_array[length];
              Serial.println("raw_array[length]");
              for (int i = 1; i <= length; i++)
              {
                fbdo.jsonArrayPtr()->get(f, 0);
                raw_array[i - 1] = f.to<uint16_t>();
              }
              // Send it out via the IR LED circuit.
              irsend.sendRaw(raw_array, length, kFrequency);
              // Deallocate the memory allocated by resultToRawArray().
              // delete[] raw_array;
              // Display a crude timestamp & notification.
              uint32_t now = millis();
              Serial.printf(
                  "%06u.%03u: A message that was %d entries long was retransmitted.\n",
                  now / 1000, now % 1000, length);
            }
            else
            {
              digitalWrite(2, LOW);
            }
          }
        }
      }
      xSemaphoreGive(xBinarySemaphore);
    }
    vTaskDelay(50);
  }
}

void irreverce(void *paramter)
{
  irrecv.enableIRIn();
  irsend.begin();
  uint16_t raw_array[500];
  for (;;)
  {
    Firebase.getArray(fbdo, "array/test");
    Serial.println("Firebase.getArray");
    Serial.println("Firebase.arrPtr");
    fbdo.jsonArrayPtr()->get(f, 0);
    Serial.println("arrPtr->get(f, 0)");
    length = f.to<uint16_t>();
    Serial.println("to uint");
    Serial.println("raw_array[length]");
    for (int i = 1; i <= length; i++)
    {
      fbdo.jsonArrayPtr()->get(f, i);
      raw_array[i - 1] = f.to<uint16_t>();
    }
    // Send it out via the IR LED circuit.
    irsend.sendRaw(raw_array, length, kFrequency);
    // Deallocate the memory allocated by resultToRawArray().
    // Display a crude timestamp & notification.
    uint32_t now = millis();
    Serial.printf(
        "%06u.%03u: A message that was %d entries long was retransmitted.\n",
        now / 1000, now % 1000, length);
    vTaskDelay(50);
  }
}