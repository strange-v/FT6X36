#include <Arduino.h>
#include <Wire.h>
#include <FT6X36.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define TouchEvent            ( 1 << 0) //1
#define ButtonEvent           ( 1 << 1) //10

EventGroupHandle_t eg;

FT6X36 ts(&Wire, 15);

void TaskProcessTouch(void *pvParameters)
{
  for (;;)
  {
    xEventGroupWaitBits(eg, TouchEvent, pdTRUE, pdTRUE, portMAX_DELAY);
    ts.processTouch();
  }
}

void TaskProcessButton(void *pvParameters)
{
  for (;;)
  {
    xEventGroupWaitBits(eg, ButtonEvent, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.println(F("Button has been pressed"));
  }
}

void IRAM_ATTR isr()
{
  BaseType_t xHigherPriorityTaskWoken;
  xEventGroupSetBitsFromISR(eg, TouchEvent, &xHigherPriorityTaskWoken);
}

void touch(TPoint p, TEvent e)
{
  if (e != TEvent::Tap)
    return;

  if (p.x < 50 && p.y < 50) //Left top area of the screen
  {
    xEventGroupSetBits(eg, ButtonEvent);
  }

  Serial.printf("x: %d, y: %d\n", p.x, p.y);
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting..."));

  Wire.begin(21, 22, 400000U);

  eg = xEventGroupCreate();
  
  ts.begin();
  ts.registerIsrHandler(isr);
  ts.registerTouchHandler(touch);

  xTaskCreatePinnedToCore(TaskProcessTouch, "fTaskProcessTouch", 10000, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(TaskProcessButton, "fTaskProcessButton", 10000, NULL, 3, NULL, 1);
}

void loop()
{  
}
