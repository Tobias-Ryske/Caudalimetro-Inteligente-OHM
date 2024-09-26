#include "config.h"
#include "web_server.h"
#include "connect_wifi_task.h"


void logError(const String &errorMessage) {
  DateTime now;
  if (rtc.begin()) {
    now = rtc.now();
  } else {
    Serial.println("RTC not available");
    return;
  }
  
  snprintf(timeStringBuff, sizeof(timeStringBuff), "%04d/%02d/%02d %02d:%02d:%02d",
           now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

  String logEntry = String(timeStringBuff) + " - " + errorMessage + "\n";

  if (xQueueSend(logQueue, logEntry.c_str(), portMAX_DELAY) != pdPASS) {
    Serial.println("Failed to send log to queue");
  }
}

void ErrorLogTask(void *pvParameters) {
    char logEntry[256];

    while (1) {
        if (xQueueReceive(logQueue, logEntry, portMAX_DELAY) == pdPASS) {
            File file = SPIFFS.open("/errors.txt", FILE_APPEND);
            if (!file) {
                Serial.println("Failed to open error log file for writing");
                continue;
            }

            file.print(logEntry);
            file.close();
            Serial.println("Error logged: " + String(logEntry));
        }
    }
}


void logResetReason() {
  esp_reset_reason_t resetReason = esp_reset_reason();
  String reason;

  switch (resetReason) {
    case ESP_RST_POWERON: reason = "Power On"; break;
    case ESP_RST_EXT: reason = "External Pin"; break;
    case ESP_RST_SW: reason = "Software"; break;
    case ESP_RST_PANIC: reason = "Exception/Panic"; break;
    case ESP_RST_INT_WDT: reason = "Internal Watchdog"; break;
    case ESP_RST_TASK_WDT: reason = "Task Watchdog"; break;
    case ESP_RST_WDT: reason = "Other Watchdog"; break;
    case ESP_RST_DEEPSLEEP: reason = "Deep Sleep"; break;
    case ESP_RST_BROWNOUT: reason = "Brownout"; break;
    case ESP_RST_SDIO: reason = "SDIO"; break;
    default: reason = "Unknown"; break;
  }

  DateTime now;
  if (rtc.begin()) {
    now = rtc.now();
  } else {
    Serial.println("RTC not available");
    return;
  }
  
  snprintf(timeStringBuff, sizeof(timeStringBuff), "%04d/%02d/%02d %02d:%02d:%02d",
           now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

  String logEntry = String(timeStringBuff) + " - ESP32 Reset Reason: " + reason + "\n";
  logError(logEntry);
}
