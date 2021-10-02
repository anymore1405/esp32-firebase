#include "ConnectLedSignal.h"
#include "EventGroupConfig.h"

void ledSignal(void *xCreatedEventGroup)
{
    EventBits_t uxBits;
    uint state = 0;
    xEventGroupSetBits(xCreatedEventGroup, BIT_LED_START);
    while (1)
    {
        uxBits = xEventGroupWaitBits(
            xCreatedEventGroup,
            BIT_LED_START | BIT_LED_INIT_WIFI | BIT_LED_INIT_FIREBASE | BIT_LED_FIREBASE_WORKING, pdFALSE, pdFALSE, portMAX_DELAY);

        if ((uxBits & BIT_LED_START) != 0)
        {
            if (!state)
            {
                state = 1;
                digitalWrite(2, HIGH);
            }
            vTaskDelay(50);
        }
        else if ((uxBits & BIT_LED_INIT_WIFI) != 0)
        {
            digitalWrite(2, state ? HIGH : LOW);
            state = !state;
            vTaskDelay(100);
        }
        else if ((uxBits & BIT_LED_INIT_FIREBASE) != 0)
        {
            digitalWrite(2, state ? HIGH : LOW);
            state = !state;
            vTaskDelay(500);
        }
        else if ((uxBits & BIT_LED_FIREBASE_WORKING) != 0)
        {
            digitalWrite(2, state ? HIGH : LOW);
            state = !state;
            vTaskDelay(1000);
        }
        vTaskDelay(10);
    }
}