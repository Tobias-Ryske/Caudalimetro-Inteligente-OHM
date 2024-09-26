#include "config.h"

void SaveDataTask(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));

        if (flowRate != 0.0) {
            DateTime now;
            if (rtc.begin()) {
                now = rtc.now();
            } else {
                Serial.println("RTC not available");
                continue;
            }

            snprintf(timeStringBuff, sizeof(timeStringBuff), "%04d/%02d/%02d %02d:%02d:%02d",
                     now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

            Serial.printf("Fecha y hora obtenida del RTC: %s\n", timeStringBuff);

            if (WiFi.status() == WL_CONNECTED) {
                WiFiClientSecure client;
                client.setCACert(cert);
                HttpClient httpClient = HttpClient(client, serverAddress, port);
                String path = "/index.php/iotcloud/savestatus";
                String contentType = "application/json";

                uint64_t chipID = ESP.getEfuseMac();
                String chipIDString = String((uint16_t)(chipID >> 32), HEX) + String((uint32_t)(chipID), HEX);

                String dataEntry = "{\"ApiKey\":\"AXDapkey45\", \"deviceId\":\"" + chipIDString + "\", \"data\":\"" + String(timeStringBuff) + "," + String(flowRate) + "\"}";

                Serial.println("Enviando datos a la nube: " + dataEntry);

                int statusCode = -1;
                int retries = 0;
                unsigned long startAttemptTime = millis();
                while (statusCode != 200 && retries < 5 && (millis() - startAttemptTime < 15000)) {
                    httpClient.post(path, contentType, dataEntry);
                    statusCode = httpClient.responseStatusCode();
                    String response = httpClient.responseBody();

                    Serial.print("Status code: ");
                    Serial.println(statusCode);
                    Serial.print("Response: ");
                    Serial.println(response);

                    if (statusCode != 200) {
                        String errorMsg = "Error al enviar los datos. Reintentando...";
                        Serial.println(errorMsg);
                        logError(errorMsg);
                        delay(1000);
                        retries++;
                    } else {
                        Serial.println("Data sent successfully");
                    }

                    if (WiFi.status() != WL_CONNECTED) {
                        String errorMsg = "WiFi desconectado durante el envío de datos.";
                        Serial.println(errorMsg);
                        logError(errorMsg);
                        break;
                    }
                }

                if (statusCode != 200) {
                    File file = SPIFFS.open("/data.json", FILE_APPEND);
                    if (!file) {
                        String errorMsg = "Failed to open file for writing";
                        Serial.println(errorMsg);
                        logError(errorMsg);
                        continue;
                    }

                    Serial.printf("Guardando datos: %s\n", dataEntry.c_str());
                    file.println(dataEntry);
                    file.close();
                }
            } else {
                File file = SPIFFS.open("/data.json", FILE_APPEND);
                if (!file) {
                    String errorMsg = "Failed to open file for writing";
                    Serial.println(errorMsg);
                    logError(errorMsg);
                    continue;
                }

                uint64_t chipID = ESP.getEfuseMac();
                String chipIDString = String((uint16_t)(chipID >> 32), HEX) + String((uint32_t)chipID, HEX);
                String dataEntry = "{\"ApiKey\":\"AXDapkey45\", \"deviceId\":\"" + chipIDString + "\", \"data\":\"" + String(timeStringBuff) + "," + String(flowRate) + "\"}";

                Serial.printf("Guardando datos: %s\n", dataEntry.c_str());
                file.println(dataEntry);
                file.close();
            }
        } else {
            Serial.println("Caudal es cero, no se guardan datos.");
        }

        if (flowRate == 0.0 && WiFi.status() == WL_CONNECTED && SPIFFS.exists("/data.json") && !isSendTaskRunning) {
            isSendTaskRunning = true;
            xTaskCreate(SendDataTask, "Send Data Task", 8192, NULL, 1, &sendTaskHandle);
        }
    }
}
