///
/// @file soft_i2c_lib_def.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-02-14
///
/// @copyright Copyright (c) 2024
///
///
#pragma once

#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LOW  0
#define HIGH 0

#define INPUT  GPIO_MODE_INPUT
#define OUTPUT GPIO_MODE_OUTPUT_OD

#define pinPullup(pin)         gpio_set_pull_mode(cfg->pin, GPIO_PULLUP_ONLY)
#define pinMode(pin, mode)     gpio_set_direction(cfg->pin, mode)
#define digitalWrite(pin, val) gpio_set_level(cfg->pin, val)
#define digitalRead(pin)       gpio_get_level(cfg->pin)
#define delay_us(us)           esp_rom_delay_us(us)