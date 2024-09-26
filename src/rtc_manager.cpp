#include "config.h"

void UpdateRTCTask(void *pvParameters) {
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
        time_t epochTime = timeClient.getEpochTime();
        struct tm *ptm = gmtime((time_t *)&epochTime);
        DateTime now = DateTime(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        rtc.adjust(now);
        Serial.println("RTC actualizado con la hora de internet");
    } else {
        String errorMsg = "No hay conexión WiFi, no se puede actualizar el RTC";
        Serial.println(errorMsg);
        logError(errorMsg);
    }

    vTaskDelete(NULL);
}
