#ifndef DEEP_SLEEP_TASK_H
#define DEEP_SLEEP_TASK_H

#include <Arduino.h>

// Declaración de funciones
void checkDeepSleepConditionTask(void *pvParameters);
void goToDeepSleep();
void suspendTasks();
void resumeTasks();

#endif // DEEP_SLEEP_TASK_H
