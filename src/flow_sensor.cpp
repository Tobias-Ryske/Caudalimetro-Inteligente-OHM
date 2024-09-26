#include "config.h"
#include "web_server.h"
#include "connect_wifi_task.h"
#include "flow_sensor.h"
#include "deep_sleep_task.h"

void IRAM_ATTR pulseCounter() {
  flowFrequency++;
}

void resumeTasks() {
  if (saveDataTaskHandle != NULL) vTaskResume(saveDataTaskHandle);
  if (sendDataWhenFlowZeroTaskHandle != NULL) vTaskResume(sendDataWhenFlowZeroTaskHandle);
  if (sendTaskHandle != NULL) vTaskResume(sendTaskHandle);
  if (checkDeepSleepConditionTaskHandle != NULL) vTaskResume(checkDeepSleepConditionTaskHandle);
}

void FlowSensorTask(void *pvParameters) {
  int lastFlowState = LOW;
  while (1) {
    unsigned long currentMillis = millis();
    int currentFlowState = digitalRead(FLOW_SENSOR_PIN);

    if (currentFlowState == LOW && lastFlowState == HIGH) {
      flowFrequency++;
    }
    lastFlowState = currentFlowState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      flowRate = ((float)flowFrequency / CALIBRATION_FACTOR);
      flowFrequency = 0;
    }

    if (flowRate > 0.0) {
      flowDetected = true;
      lastFlowDetectedMillis = millis();
      resumeTasks();
      deepSleepMode = false; // reset deep sleep mode
    } else {
      flowDetected = false;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
