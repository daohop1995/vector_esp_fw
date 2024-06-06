/// 
/// @file daq_mod.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief 
/// @version 0.1
/// @date 2023-12-09
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

extern int daq_mod_init(void);
void audio_config();
