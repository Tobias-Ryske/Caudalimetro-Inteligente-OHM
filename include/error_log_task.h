#ifndef ERROR_LOG_TASK_H
#define ERROR_LOG_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Declaración de variables externas
extern QueueHandle_t logQueue;

// Declaración de funciones
void ErrorLogTask(void *pvParameters);
void logError(const String &errorMessage);

#endif // ERROR_LOG_TASK_H
