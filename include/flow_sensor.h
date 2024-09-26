#ifndef FLOW_SENSOR_H
#define FLOW_SENSOR_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <config.h>


// Declaración de funciones
void resumeTasks();
void pulseCounter();
void turnOnDisplay();

#endif // FLOW_SENSOR_H
