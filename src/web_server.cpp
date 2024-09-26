#include "web_server.h"
#include "connect_wifi_task.h"
#include "config.h"
#include "deep_sleep_task.h"

// Función para manejar la solicitud de archivos
void handleFileindex() {
  String path = server.uri();
  if (path.endsWith("/")) {
    path += "index.html";
  }
  String contentType = "text/html";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
    logError("File not found: " + path);
  }
}

// Función para manejar la solicitud de archivos 
void handleFileservicio() {
  String path = server.uri();
  if (path.endsWith("/")) {
    path += "servicio.html";
  }
  String contentType = "text/html";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
    logError("File not found: " + path);
  }
}

// Función para manejar la solicitud de archivos
void handleFileusuario() {
  String path = server.uri();
  if (path.endsWith("/")) {
    path += "usuario.html";
  }
  String contentType = "text/html";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
    logError("File not found: " + path);
  }
}

// Función para manejar la configuración de WiFi
void handleWiFiConfig() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String newSsid = server.arg("ssid");
    String newPassword = server.arg("password");

    preferences.putString("ssid", newSsid);
    preferences.putString("password", newPassword);

    ssid = newSsid;
    password = newPassword;

    server.send(200, "text/plain", "WiFi credentials saved. Reconnecting...");
    connectToWiFi();
  } else {
    server.send(400, "text/plain", "Bad Request");
    logError("Bad Request: Missing ssid or password");
  }
}

// Función para manejar la eliminación de errores
void handleDeleteErrors() {
  if (SPIFFS.exists("/errors.txt")) {
    SPIFFS.remove("/errors.txt");
    server.send(200, "text/plain", "Archivo de errores borrado.");
  } else {
    server.send(404, "text/plain", "Archivo no encontrado.");
  }
}

// Función para manejar la solicitud de estado
void handleStatus() {
  StaticJsonDocument<200> jsonDoc;
  uint64_t chipID = ESP.getEfuseMac();
  String chipIDString = String((uint16_t)(chipID >> 32), HEX) + String((uint32_t)(chipID), HEX);
  
  jsonDoc["chipID"] = chipIDString;
  jsonDoc["flowRate"] = flowRate;
  jsonDoc["factor"] = CALIBRATION_FACTOR;
  // Leer el intervalo de sueño desde la memoria flash
  unsigned long storedSleepInterval;
  loadSleepConfig(storedSleepInterval);
  jsonDoc["sleeptime"] = storedSleepInterval / 1000; // Convertir a segundos para enviar al cliente

  
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

// Función para manejar el reinicio del dispositivo
void handleReboot() {
  server.send(200, "text/plain", "Rebooting...");
  delay(1000);
  ESP.restart();
}

// Función para manejar la descarga de errores
void handleDownloadErrors() {
  if (SPIFFS.exists("/errors.txt")) {
    File file = SPIFFS.open("/errors.txt", "r");
    server.streamFile(file, "text/plain");
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
    logError("File not found: /errors.txt");
  }
}

// Función para guardar el factor de calibración en SPIFFS
void saveCalibrationFactor() {
  if (SPIFFS.exists("/calibration.json")) {
    SPIFFS.remove("/calibration.json");
  }

  File file = SPIFFS.open("/calibration.json", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open calibration file for writing");
    return;
  }

  DynamicJsonDocument doc(1024);
  doc["factor"] = CALIBRATION_FACTOR;
  serializeJson(doc, file);
  file.close();

  Serial.print("Calibration factor saved: ");
  Serial.println(CALIBRATION_FACTOR);
}

// Función para manejar la configuración de calibración
void handleSetCalibration() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, body);

    if (doc.containsKey("factor")) {
      CALIBRATION_FACTOR = doc["factor"].as<float>();
      saveCalibrationFactor();
      server.send(200, "text/plain", "Calibración establecida.");
    } else {
      server.send(400, "text/plain", "Solicitud inválida.");
    }
  } else {
    server.send(400, "text/plain", "Solicitud inválida.");
  }
}

// Función para cargar el factor de calibración desde SPIFFS
void loadCalibrationFactor() {
  if (SPIFFS.exists("/calibration.json")) {
    File file = SPIFFS.open("/calibration.json", FILE_READ);
    if (!file) {
      Serial.println("Failed to open calibration file for reading");
      return;
    }

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, file);
    CALIBRATION_FACTOR = doc["factor"].as<float>();
    file.close();

    Serial.print("Calibration factor loaded: ");
    Serial.println(CALIBRATION_FACTOR);
  } else {
    Serial.println("Calibration file not found, using default factor");
    CALIBRATION_FACTOR = 7.5; // Valor por defecto
  }
}

void handleSetSleepConfig() {
    if (server.hasArg("plain")) {
        String body = server.arg("plain");
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, body);

        if (doc.containsKey("interval")) {
            unsigned long interval = doc["interval"];
            sleepInterval = interval * 1000; // Convertir segundos a milisegundos
            preferences.putULong("sleepInterval", sleepInterval);
            saveSleepConfig(sleepInterval);
            server.send(200, "text/plain", "Configuración de sueño actualizada.");
        } else {
            server.send(400, "text/plain", "Solicitud inválida.");
        }
    } else {
        server.send(400, "text/plain", "Solicitud inválida.");
    }
}

void handleGetSleepConfig() {   
    DynamicJsonDocument doc(1024); 
    loadSleepConfig(sleepInterval);
    doc["interval"] = sleepInterval / 1000; // Convertir milisegundos a segundos

    String jsonString;
    serializeJson(doc, jsonString);

    server.send(200, "application/json", jsonString);
}
                                                

void saveSleepConfig(unsigned long sleepInterval) {
    DynamicJsonDocument doc(1024);
    doc["interval"] = sleepInterval;

    File file = SPIFFS.open("/sleepConfig.json", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open sleepConfig file for writing");
        return;
    }

    serializeJson(doc, file);
    file.close();

    Serial.println("Sleep configuration saved");
}

void loadSleepConfig(unsigned long &sleepInterval) {
    if (SPIFFS.exists("/sleepConfig.json")) {
        File file = SPIFFS.open("/sleepConfig.json", FILE_READ);
        if (!file) {
            Serial.println("Failed to open sleepConfig file for reading");
            return;
        }

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, file);

        sleepInterval = doc["interval"].as<unsigned long>();

        file.close();
    } else {
        Serial.println("Sleep configuration file not found, using defaults");
        sleepInterval = 60000; // 60 segundos por defecto
    }
}

// Tarea para manejar el servidor web
void WebServerTask(void *pvParameters) {
  while (1) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Función para configurar las rutas del servidor
void setupServerRoutes() {
  server.on("/", HTTP_GET, handleFileindex);
  server.on("/servicio.html", HTTP_GET, handleFileservicio);
  server.on("/usuario.html", HTTP_GET, handleFileusuario);
  server.on("/configWiFi", HTTP_POST, handleWiFiConfig);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/reboot", HTTP_POST, handleReboot);
  server.on("/downloadErrors", HTTP_GET, handleDownloadErrors);
  server.on("/deleteErrors", HTTP_POST, handleDeleteErrors);
  server.on("/setCalibration", HTTP_POST, handleSetCalibration);
  server.on("/setSleepConfig", HTTP_POST, handleSetSleepConfig);
  server.on("/getSleepConfig", HTTP_GET, handleGetSleepConfig);
  server.onNotFound(handleFileindex);
  server.begin();
  Serial.println("HTTP server started");
}

