///
/// @file bioexp.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-03-17
///
/// @copyright Copyright (c) 2024
///
///
#include "bioexp.h"
#include "hal/bi2c/bi2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "hIOX";

static const struct bi2c_slave ioexp_slave = {
    .id = BI2C_1,
    .addr = 0x20,
    .read_mode = BI2C_RESTART,
    .write_mode = BI2C_NO_RESTART,
};

TaskHandle_t task;
static struct {
    uint8_t inp_en;
    uint8_t inp_state;
    uint8_t out_state;
} _rv = {
    .inp_en = 0xFF,
    .inp_state = 0,
    .out_state = 0xFF,
};

SLL_LISTENER_IS_VALID_CHECK(bioexp)
{
    return true;
}

SLL_LISTENER_DEFINE(bioexp);

int bioexp_mode(enum bioexp_ios io, bool inp)
{
    WRITE_BIT(_rv.inp_en, io, inp);
    return bi2c_write_reg_u8(&ioexp_slave, 0x03, _rv.inp_en);
}

int bioexp_get(enum bioexp_ios io)
{
    return ((_rv.inp_en & BIT(io))
                ? _rv.inp_state & BIT(io)
                : _rv.out_state & BIT(io)) > 0;
}

int bioexp_set(enum bioexp_ios io, int val)
{
    WRITE_BIT(_rv.out_state, io, val);
    return bi2c_write_reg_u8(&ioexp_slave, 0x01, _rv.inp_en);
}

void bioexp_int_handler(void *arg)
{
    xTaskNotifyFromISR(task, 1, eSetBits, pdFALSE);
}

void bioexp_task_function(void *pvParameters)
{
    uint32_t notifValue;
    // on entry fire with default values
    SLL_LISTENER_FIRE_NODES(bioexp, on_change, 0xFF, _rv.inp_state);
    //
    while (true) {
        xTaskNotifyWait(0x00, ULONG_MAX, &notifValue, portMAX_DELAY);
        //
        uint8_t new_inp_val = bi2c_read_reg_u8(&ioexp_slave, 0x00);
        uint8_t toggled = (_rv.inp_state ^ new_inp_val) & _rv.inp_en;
        _rv.inp_state = new_inp_val;
        //
        if (toggled) {
            SLL_LISTENER_FIRE_NODES(bioexp, on_change, toggled, _rv.inp_state);
        }
    }
}

int bioexp_init()
{
    // default state of driver is input and default state of input should be input too
    // thus we only need to setup read on interrupt handling
    _rv.inp_state = bi2c_read_reg_u8(&ioexp_slave, 0x00);
    
    // zero-initialize the config structure.
    gpio_config_t io_conf = {0};
    // interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = BIT(GPIO_NUM_11);
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    // change gpio interrupt type for one pin
    gpio_set_intr_type(GPIO_NUM_11, GPIO_INTR_NEGEDGE);
    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_NUM_11, bioexp_int_handler, (void *)GPIO_NUM_11);
    // start gpio task
    xTaskCreate(bioexp_task_function, "bioexp_task", 2048, NULL, 10, &task);
    return 0;
}
