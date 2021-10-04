#ifndef LED_H
#define LED_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

enum LED_Control_Vals
{
    OFF = 0,
    ON = 1,
    BLINK = 2
};

typedef enum LED_Control_Vals LED_Control_Vals_t;

#define LED_BUILTIN GPIO_NUM_33
#define LED_BLINK_DEFAULT_PERIOD 1000

LED_Control_Vals_t gLED_BUILTIN_STATE;
unsigned int gLED_BUILTIN_PERIOD;

void io_init();
esp_err_t LED_controller(int state, int period_ms);

#endif /* LED_H */
