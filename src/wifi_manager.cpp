#include <WiFi.h>
#include "config.h"
#include "connect_wifi_task.h"

void connectToWiFi() {
  if (ssid.length() > 0) {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to WiFi");
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(100);
      Serial.print(".");
      yield();
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      String errorMsg = "Failed to connect to WiFi";
      Serial.println(errorMsg);
      logError(errorMsg);
    }
  }
}


