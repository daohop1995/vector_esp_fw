/**
 * @file si7006.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-24
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

/*Humidity Sensor*/
#define I2C_HUMI_SENSOR_ADDRESS      0x40
#define CMD_HOLD_MASTER_MODE_H       0xE5
#define CMD_HOLD_NO_MASTER_MODE_H    0xF5
#define CMD_HOLD_MASTER_MODE_T       0xE3
#define CMD_HOLD_NO_MASTER_MODE_T    0xF3
#define CMD_READ_TEMP_PREV_VALUE     0xE0
#define CMD_RESET_TEMP               0xFE
#define CMD_WRITE_RH_T_USER_REG1     0xE6
#define CMD_READ_RH_T_USER_REG1      0xE7
#define CMD_WRITE_HEATER_CONTROL_REG 0x51
#define CMD_READ_HEATER_CONTROL_REG  0x11
#define CMD_READ_ELECT_ID_BYTE1      0xFA
#define CMD_READ_ELECT_ID_BYTE2      0xFC
#define CMD_READ_FIRMWARE_REV        0x84

struct si7006_sample_value {
    uint16_t humidity;   // % 0 - 100
    int16_t temperature; // x100
};

struct si7006_settings {
    /* data */
};