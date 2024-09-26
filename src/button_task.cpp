#include "config.h"
#include "flow_sensor.h"
#include "save_data_task.h"
#include "send_data_task.h"
#include "error_log_task.h"
#include "deep_sleep_task.h"
#include "display_manager.h"
#include "rtc_manager.h"
#include "web_server.h"
#include "connect_wifi_task.h"

void wakeupFromSleep() {
  if (deepSleepMode) {
    deepSleepMode = false;
    turnOnDisplay();
    resumeTasks();
    logError("Wakeup from sleep due to button press.");
  }
}

void ButtonTask(void *pvParameters) {
    while (1) {
        if (digitalRead(BUTTON_PIN_4) == LOW) {
            displayFlowRate = true;
            wakeupFromSleep();
        } else if (digitalRead(BUTTON_PIN_32) == LOW) {
            displayFlowRate = false;
            wakeupFromSleep();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
