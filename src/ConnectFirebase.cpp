#include <IRsend.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "ConnectFirebase.h"
#include "EventGroupConfig.h"
#include "Constant.h"
#include <ESPRandom.h>
#include <Preferences.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "WIFI 303"
#define WIFI_PASSWORD "500kchopass"

FirebaseData fbdo;
FirebaseData stream;

String path = "/test/int";

FirebaseAuth auth;
FirebaseConfig config;
FirebaseJsonData f;

const uint32_t kBaudRate = 115200;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 50;
const uint16_t kFrequency = 38000;
uint16_t length;

IRsend irsend(IR_LED_PIN);
IRrecv irrecv(IR_RV_PIN, kCaptureBufferSize, kTimeout, false);
decode_results results;

void initFirebase(void *xCreatedEventGroup)
{
  EventBits_t uxBits;
  Preferences preferences;
  preferences.begin("data", true);
  for (;;)
  {
    uxBits = xEventGroupWaitBits(
        xCreatedEventGroup,
        BIT_INIT_FIREBASE | BIT_INIT_WIFI | BIT_FIREBASE_LISTENER, pdFALSE, pdFALSE, portMAX_DELAY);

    if ((uxBits & BIT_INIT_WIFI) != 0)
    {
      xEventGroupClearBits(xCreatedEventGroup, BIT_LED_START);

      preferences.begin("data", false);
      Serial.println(preferences.getString(UUID));
      if (preferences.getString(UUID).isEmpty())
      {
        Serial.println("write uuid");
        uint8_t uuid[16];
        ESPRandom::uuid(uuid);
        preferences.putString(UUID, ESPRandom::uuidToString(uuid));
        preferences.putString(WIFI_SSID_KEY, WIFI_SSID);
        preferences.putString(WIFI_PASSWORD_KEY, WIFI_PASSWORD);
        preferences.putString(FIREBASE_KEY, "khactai14052000@gmail.com");
        preferences.putString(FIREBASE_PASSWORD_KEY, "123456");
      }
      WiFi.begin(preferences.getString(WIFI_SSID_KEY).c_str(), preferences.getString(WIFI_PASSWORD_KEY).c_str());
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
      xEventGroupClearBits(xCreatedEventGroup, BIT_LED_INIT_WIFI);
      xEventGroupClearBits(xCreatedEventGroup, BIT_INIT_WIFI);
      xEventGroupSetBits(xCreatedEventGroup, BIT_INIT_FIREBASE);
      xEventGroupSetBits(xCreatedEventGroup, BIT_LED_INIT_FIREBASE);
    }
    else if ((uxBits & BIT_INIT_FIREBASE) != 0)
    {
      Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

      config.api_key = "AIzaSyBuwoPc2k1LLwb0Rg0CQ5R3B-kU2G99Tz4";
      
      Serial.println(preferences.getString(FIREBASE_KEY).c_str());
      Serial.println(preferences.getString(FIREBASE_PASSWORD_KEY).c_str());
      auth.user.email = preferences.getString(FIREBASE_KEY).c_str();
      auth.user.password = preferences.getString(FIREBASE_PASSWORD_KEY).c_str();
      config.database_url = DATABASE_URL;
      config.token_status_callback = tokenStatusCallback;

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
      xEventGroupSetBits(xCreatedEventGroup, BIT_FIREBASE_LISTENER);
      xEventGroupSetBits(xCreatedEventGroup, BIT_LED_FIREBASE_WORKING);
      xEventGroupClearBits(xCreatedEventGroup, BIT_INIT_FIREBASE);
      xEventGroupClearBits(xCreatedEventGroup, BIT_LED_INIT_FIREBASE);
    }
    else if ((uxBits & BIT_FIREBASE_LISTENER) != 0)
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
              xEventGroupSetBits(xCreatedEventGroup, BIT_IR_SEND);
            }
            else
            {
              xEventGroupSetBits(xCreatedEventGroup, BIT_IR_RV);
            }
          }
        }
      }
    }
    vTaskDelay(50);
  }
}

void irreverce(void *xCreatedEventGroup)
{
  irrecv.enableIRIn();
  irsend.begin();
  uint16_t raw_array[500];
  EventBits_t uxBits;
  for (;;)
  {
    uxBits = xEventGroupWaitBits(
        xCreatedEventGroup,
        BIT_IR_SEND | BIT_IR_RV, pdTRUE, pdFALSE, portMAX_DELAY);

    if ((uxBits & BIT_IR_SEND) != 0)
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
    }
    if ((uxBits & BIT_IR_RV) != 0)
    {
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

    vTaskDelay(50);
  }
}