#include "config.h"
#include "web_server.h"
#include "connect_wifi_task.h"
#include "deep_sleep_task.h"


void suspendTasks() {
  if (saveDataTaskHandle != NULL) vTaskSuspend(saveDataTaskHandle);
  if (sendDataWhenFlowZeroTaskHandle != NULL) vTaskSuspend(sendDataWhenFlowZeroTaskHandle);
  if (sendTaskHandle != NULL) vTaskSuspend(sendTaskHandle);
  if (checkDeepSleepConditionTaskHandle != NULL) vTaskSuspend(checkDeepSleepConditionTaskHandle);
}

void goToDeepSleep() {
  display.clearDisplay();
  display.display();
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  delay(100);

  // Configurar el pin de flujo como fuente de interrupción (ext0)
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_15, 1); // Interrupción por flujo de agua

  // Configurar los pines de los botones como fuentes de interrupción (ext1)
  esp_sleep_enable_ext1_wakeup((1ULL << BUTTON_PIN_4) | (1ULL << BUTTON_PIN_32), ESP_EXT1_WAKEUP_ANY_HIGH);

  esp_deep_sleep_start();
}

void checkDeepSleepConditionTask(void *pvParameters) {
  unsigned long sleepInterval;
  unsigned long flowZeroTime = 0;

  // Cargar la configuración del intervalo de sueño
  loadSleepConfig(sleepInterval);

  while (1) {
    if (!flowDetected) {
      if (flowZeroTime == 0) {
        flowZeroTime = millis();
      } else if (millis() - flowZeroTime > sleepInterval) {
        logError("No se detectó flujo durante el intervalo configurado, entrando en modo de sueño.");
        deepSleepMode = true;
        suspendTasks();
        goToDeepSleep();
      }
    } else {
      flowZeroTime = 0;
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
