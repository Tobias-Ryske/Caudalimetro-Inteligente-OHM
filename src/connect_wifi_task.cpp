#include "config.h"
#include "web_server.h"
#include "connect_wifi_task.h"

void ConnectToWiFiTask(void *pvParameters) {
    while (1) {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect();
            connectToWiFi();
        } else {
            if (flowRate == 0.0 && SPIFFS.exists("/data.json") && !isSendTaskRunning) {
                isSendTaskRunning = true;
                xTaskCreate(SendDataTask, "Send Data Task", 8192, NULL, 1, &sendTaskHandle);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}

void ConnectToWiFiDirectTask(void *pvParameters) {
    WiFi.softAP("OHM Instrumental", "12345678");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP Address: ");
    Serial.println(IP);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}
