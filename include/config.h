#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Preferences.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTClib.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Pines y constantes
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SSD1306_I2C_ADDRESS 0x3C
extern Adafruit_SSD1306 display;

#define BUTTON_PIN_4 4
#define BUTTON_PIN_32 32

extern const int FLOW_SENSOR_PIN;
extern volatile int flowFrequency;
extern float CALIBRATION_FACTOR;

// Declaración de variables globales para el modo de sueño
extern unsigned long sleepInterval;

extern unsigned long previousMillis;
extern const unsigned long interval;
extern float flowRate;
extern bool flowDetected;
extern unsigned long lastFlowDetectedMillis;
extern bool deepSleepMode;

extern char timeStringBuff[20];
extern bool displayFlowRate;

extern Preferences preferences;
extern String ssid;
extern String password;

extern WiFiUDP ntpUDP;
extern NTPClient timeClient;

extern RTC_DS3231 rtc;

extern WebServer server;

extern QueueHandle_t logQueue;

// Declaración de tareas freertos

void FlowSensorTask(void *pvParameters);
void SaveDataTask(void *pvParameters);
void WebServerTask(void *pvParameters);
void SendDataTask(void *pvParameters);
void ConnectToWiFiTask(void *pvParameters);
void ConnectToWiFiDirectTask(void *pvParameters);
void DisplayTask(void *pvParameters);
void ButtonTask(void *pvParameters);
void UpdateRTCTask(void *pvParameters);
void SendDataWhenFlowZeroTask(void *pvParameters);
void ErrorLogTask(void *pvParameters);
void logError(const String &errorMessage);
void checkDeepSleepConditionTask(void *pvParameters);

// declaraciones de funciones

void logResetReason();
void goToDeepSleep();
void wakeupFromSleep();
void sendDataIfAvailable();

void saveSleepConfig(unsigned long sleepInterval);
void loadSleepConfig(unsigned long &sleepInterval);   

extern const char* serverAddress;
extern const int port;

extern const char* cert;

extern bool isSendTaskRunning;

extern TaskHandle_t sendTaskHandle;
extern TaskHandle_t saveDataTaskHandle;
extern TaskHandle_t sendDataWhenFlowZeroTaskHandle;
extern TaskHandle_t flowSensorTaskHandle;
extern TaskHandle_t checkDeepSleepConditionTaskHandle;
extern TaskHandle_t buttonTaskHandle;

#endif // CONFIG_H


