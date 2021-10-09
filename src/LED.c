#ifndef LED_C
#define LED_C

#include "LED.h"

#define LED_BLINK_TASK_STACK_SIZE 500

void io_init()
{
    // Config RED LED built in
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = 1ULL << GPIO_NUM_33;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(LED_BUILTIN, 0);
    gLED_BUILTIN_STATE = ON;

    // Config Flash light LED
    io_conf.pin_bit_mask = 1ULL << GPIO_NUM_4;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Another way to configure the GPIO

    // gpio_pad_select_gpio(LED_BUILTIN);
    /* Set the GPIO as a push/pull output */
    // gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
}

static void LED_blink_task(void *pvParameters)
{

    while (1)
    {
        gpio_set_level(LED_BUILTIN, 0);
        vTaskDelay(gLED_BUILTIN_PERIOD / portTICK_PERIOD_MS);
        gpio_set_level(LED_BUILTIN, 1);
        vTaskDelay(gLED_BUILTIN_PERIOD / portTICK_PERIOD_MS);
    }
}

esp_err_t LED_controller(int state, int period_ms)
{
    gLED_BUILTIN_PERIOD = LED_BLINK_DEFAULT_PERIOD;
    static bool isBlinkTaskCreated = 0;
    static bool isBlinking = 1;
    static TaskHandle_t xLEDBlinkTaskHandle;

    if (isBlinkTaskCreated == 0)
    {
        if (xTaskCreate(LED_blink_task, "LED_Blink_Task", LED_BLINK_TASK_STACK_SIZE,
                        NULL, 5, &xLEDBlinkTaskHandle) == pdPASS)
            isBlinkTaskCreated = 1;
        else
            return ESP_ERR_NO_MEM;
    }

    if (isBlinking)
    {
        vTaskSuspend(xLEDBlinkTaskHandle);
    }

    // Blink
    if (period_ms > 0)
    {

        vTaskResume(xLEDBlinkTaskHandle);
        isBlinking = 1;
        gLED_BUILTIN_STATE = BLINK;
    }

    // Turn off
    else if (state == OFF)
    {
        gpio_set_level(LED_BUILTIN, 1);
        gLED_BUILTIN_STATE = OFF;
    }

    // Turn on
    else if (state == ON)
    {
        gpio_set_level(LED_BUILTIN, 0);
        gLED_BUILTIN_STATE = ON;
    }

    // Not supported
    else
        return ESP_ERR_NOT_SUPPORTED;

    return ESP_OK;
}

#endif /* LED_C */
