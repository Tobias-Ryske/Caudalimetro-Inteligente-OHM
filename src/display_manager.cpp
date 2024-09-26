#include "config.h"
#include "web_server.h"
#include "connect_wifi_task.h"

void turnOnDisplay() {
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void DisplayTask(void *pvParameters) {
    while (1) {
        if (deepSleepMode) {
            display.ssd1306_command(SSD1306_DISPLAYOFF);
        } else {
            display.clearDisplay();
            display.setCursor(0, 0);

            if (WiFi.status() == WL_CONNECTED) {
                display.print("WiFi: Conectado");
            } else {
                display.print("WiFi: Desconectado");
            }

            display.setCursor(0, 20);
            if (displayFlowRate) {
                display.print("Caudal: ");
                display.print(flowRate);
                display.print(" L/min");
            } else {
                display.print("IP WiFi Direct:");
                display.setCursor(0, 40);
                display.print(WiFi.softAPIP());
            }

            display.display();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
