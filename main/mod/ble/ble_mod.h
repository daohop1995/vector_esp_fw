/// 
/// @file ble_mod.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief 
/// @version 0.1
/// @date 2023-12-16
/// 
/// @copyright Copyright (c) 2023
/// 
/// 
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

int ble_mod_init(void);

#ifdef __cplusplus
}
#endif