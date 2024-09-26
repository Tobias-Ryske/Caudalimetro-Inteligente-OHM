#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "config.h"

void handleFileusuario();
void handleFileservicio(); 
void handleFileindex();
void handleWiFiConfig();
void handleDeleteErrors();
void handleStatus();
void handleReboot();
void handleDownloadErrors();
void handleSetCalibration();
void saveCalibrationFactor();
void loadCalibrationFactor();
void WebServerTask(void *pvParameters);
void setupServerRoutes();

void handleSetSleepConfig();
void handleGetSleepConfig(); 
void saveSleepConfig(unsigned long sleepInterval);
void loadSleepConfig(unsigned long &sleepInterval);

#endif // WEB_SERVER_H
