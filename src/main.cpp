#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "flow_sensor.h"
#include "save_data_task.h"
#include "send_data_task.h"
#include "error_log_task.h"
#include "deep_sleep_task.h"
#include "display_manager.h"
#include "rtc_manager.h"
#include "web_server.h"
#include "connect_wifi_task.h"


void setup() {
    Serial.begin(9600);
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);

    pinMode(BUTTON_PIN_4, INPUT_PULLUP);
    pinMode(BUTTON_PIN_32, INPUT_PULLUP);

    xTaskCreate(FlowSensorTask, "Flow Sensor Task", 4096, NULL, 1, &flowSensorTaskHandle);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS)) {
        String errorMsg = "SSD1306 allocation failed";
        Serial.println(errorMsg);
        logError(errorMsg);
        for(;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 10);
    display.println(F("Caudalimetro"));
    display.display();

    if (!SPIFFS.begin(true)) {
        String errorMsg = "An error has occurred while mounting SPIFFS";
        Serial.println(errorMsg);
        logError(errorMsg);
        return;
    }

    loadCalibrationFactor(); // Cargar el factor de calibración al inicio

    preferences.begin("wifi", false);
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");

    if (!rtc.begin()) {
        String errorMsg = "Couldn't find RTC";
        Serial.println(errorMsg);
        logError(errorMsg);
    }

    if (rtc.lostPower() || esp_reset_reason() == ESP_RST_POWERON) {
        Serial.println("RTC lost power, let's set the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    logQueue = xQueueCreate(10, sizeof(char) * 256);
    if (logQueue == NULL) {
        Serial.println("Failed to create log queue");
        return;
    }
    xTaskCreate(ErrorLogTask, "Error Log Task", 4096, NULL, 1, NULL);

    logResetReason();

    xTaskCreate(ConnectToWiFiTask, "Connect To WiFi Task", 8192, NULL, 1, NULL);
    vTaskDelay(pdMS_TO_TICKS(5000));
    xTaskCreate(ConnectToWiFiDirectTask, "Connect To WiFi Direct Task", 8192, NULL, 1, NULL);

    xTaskCreate(DisplayTask, "Display Task", 4096, NULL, 1, NULL);
    xTaskCreate(ButtonTask, "Button Task", 2048, NULL, 1, &buttonTaskHandle);

    xTaskCreate(UpdateRTCTask, "Update RTC Task", 4096, NULL, 1, NULL);

    xTaskCreate(SaveDataTask, "Save Data Task", 8192, NULL, 1, &saveDataTaskHandle);

    xTaskCreate(SendDataWhenFlowZeroTask, "Send Data When Flow Zero Task", 8192, NULL, 1, &sendDataWhenFlowZeroTaskHandle);

    xTaskCreate(checkDeepSleepConditionTask, "Check Deep Sleep Condition Task", 4096, NULL, 1, &checkDeepSleepConditionTaskHandle);

    xTaskCreate(WebServerTask, "Web Server Task", 4096, NULL, 1, NULL);

    setupServerRoutes();

    Serial.println("Inicio de medición del caudal");
}

void loop() {
    // El loop está vacío porque las tareas FreeRTOS se encargan de todo
}



