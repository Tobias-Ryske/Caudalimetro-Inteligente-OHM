#include "config.h"

void SendDataTask(void *pvParameters) {
    WiFiClientSecure client;
    client.setCACert(cert);
    HttpClient httpClient = HttpClient(client, serverAddress, port);
    String path = "/index.php/iotcloud/savestatus";
    String contentType = "application/json";

    if (SPIFFS.exists("/data.json")) {
        File file = SPIFFS.open("/data.json", FILE_READ);
        if (!file) {
            String errorMsg = "Failed to open file for reading";
            Serial.println(errorMsg);
            logError(errorMsg);
            isSendTaskRunning = false;
            vTaskDelete(NULL);
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            Serial.println("Ejecutando POST request");

            int statusCode = -1;
            int retries = 0;
            unsigned long startAttemptTime = millis();
            while (statusCode != 200 && retries < 5 && (millis() - startAttemptTime < 15000)) {
                httpClient.post(path, contentType, line);
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

                if (flowRate > 0.0) {
                    String errorMsg = "Flow detected during data send, stopping SendDataTask.";
                    Serial.println(errorMsg);
                    logError(errorMsg);
                    file.close();
                    isSendTaskRunning = false;
                    vTaskDelete(NULL);
                }
            }

            if (statusCode != 200) {
                String errorMsg = "Failed to send data after retries or timeout. Stopping task.";
                Serial.println(errorMsg);
                logError(errorMsg);
                file.close();
                isSendTaskRunning = false;
                vTaskDelete(NULL);
            }
        }

        file.close();
        SPIFFS.remove("/data.json");
        Serial.println("Todos los datos han sido enviados y el archivo ha sido eliminado.");
    } else {
        String errorMsg = "No se encontró el archivo /data.json";
        Serial.println(errorMsg);
        logError(errorMsg);
    }

    isSendTaskRunning = false;
    vTaskDelete(NULL);
}

void SendDataWhenFlowZeroTask(void *pvParameters) {
  while (1) {
    if (flowRate == 0.0 && WiFi.status() == WL_CONNECTED && SPIFFS.exists("/data.json") && !isSendTaskRunning) {
      isSendTaskRunning = true;
      xTaskCreate(SendDataTask, "Send Data Task", 8192, NULL, 1, &sendTaskHandle);
    }
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
