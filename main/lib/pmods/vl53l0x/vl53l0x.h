///
/// @file VL53L0X.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-07-17
///
/// @copyright Copyright (c) 2022
///
///
#pragma once

#include <stdint.h>
#include <stdbool.h>

struct vl53l0x_stm {
    const void *slave;
    bool did_timeout;
    uint8_t last_status;   // status of last I2C transmission
    uint8_t stop_variable; // read by init and used when starting measurement; is StopVariable field of VL53L0X_DevData_t structure in API
    uint16_t io_timeout;
    uint16_t timeout_start_ms;
    uint32_t measurement_timing_budget_us;
};

struct vl53l0x_sample_value {
    uint16_t proximity;
};

struct vl53l0x_settings {
    /* data */
};