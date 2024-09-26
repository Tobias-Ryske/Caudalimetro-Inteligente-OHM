#ifndef SEND_DATA_TASK_H
#define SEND_DATA_TASK_H

#include <Arduino.h>

// Declaración de funciones
void SendDataTask(void *pvParameters);
void SendDataWhenFlowZeroTask(void *pvParameters);

#endif // SEND_DATA_TASK_H
