#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <Arduino.h>

// Declaración de funciones
void UpdateRTCTask(void *pvParameters);
void logResetReason();

#endif // RTC_MANAGER_H
