#include "ConnectFirebase.h"
FirebaseData fbdo;
FirebaseData stream;

String path = "/test/int";

FirebaseAuth auth;
FirebaseConfig config;

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
  }
}