///
/// @file ble_mod.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-12-16
///
/// @copyright Copyright (c) 2023
///
///
#include "mod/ble/ble_mod.h"
#include "hal/bble/bble.h"

#define BLE_TASK_STACK_SIZE 4096

void on_bble_data_written(const uint8_t *data, uint16_t data_len)
{
}

/* Task to be created. */
void ble_task_function(void *pvParameters)
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );*/
    bble_init(on_bble_data_written);
    for (;;) {
        /* Task code goes here. */
        vTaskDelay(portMAX_DELAY);
    }
}

int ble_mod_init()
{
    BaseType_t xReturned;
    TaskHandle_t xDaqHandle = NULL;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
        ble_task_function,   /* Function that implements the task. */
        "BLE",               /* Text name for the task. */
        BLE_TASK_STACK_SIZE, /* Stack size in words, not bytes. */
        (void *)NULL,        /* Parameter passed into the task. */
        tskIDLE_PRIORITY,    /* Priority at which the task is created. */
        &xDaqHandle);        /* Used to pass out the created task's handle. */

    if (xReturned != pdPASS) {
        /* The task was created.  Use the task's handle to delete the task. */
        vTaskDelete(xDaqHandle);
    }
    return xReturned;
}
