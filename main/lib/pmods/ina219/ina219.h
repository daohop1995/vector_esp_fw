/**
 * @file ina219.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include "hal/bi2c/bi2c.h"

#define INA219_I2CADDR_DEFAULT 0x40 ///< INA219 default i2c address

#define INA219_REG_CONFIG      0x00 ///< Configuration register
#define INA219_REG_CURRENT     0x01 ///< Current measurement register (signed) in mA
#define INA219_REG_BUSVOLTAGE  0x02 ///< Bus voltage measurement register in mV
#define INA219_REG_POWER       0x03 ///< Power calculation register in mW
#define INA219_REG_MASK_ENABLE 0x06 ///< Interrupt/Alert setting and checking register
#define INA219_REG_ALERT_LIMIT 0x07 ///< Alert limit value register
#define INA219_REG_MFG_UID     0xFE ///< Manufacturer ID Register
#define INA219_REG_DIE_UID     0xFF ///< Die ID and Revision Register

typedef enum {
    INA219_AVG_1,
    INA219_AVG_4,
    INA219_AVG_16,
    INA219_AVG_64,
    INA219_AVG_128,
    INA219_AVG_256,
    INA219_AVG_512,
    INA219_AVG_1024
} b_ina219_avg_cfg_t;

typedef enum {
    INA219_TRIG_OFF,
    INA219_TRIG_I,
    INA219_TRIG_V,
    INA219_TRIG_IV,
    INA219_CONT_OFF,
    INA219_CONT_I,
    INA219_CONT_V,
    INA219_CONT_IV,
} b_ina219_mode_cfg_t;

typedef enum {
    INA219_CONV_140_US,
    INA219_CONV_204_US,
    INA219_CONV_332_US,
    INA219_CONV_588_US,
    INA219_CONV_1100_US,
    INA219_CONV_2116_US,
    INA219_CONV_4156_US,
    INA219_CONV_8244_US
} b_ina219_convtime_cfg_t;

struct ina219_sample_value {
    uint16_t voltage; // volts across bus in mV
    uint16_t current; // volt across shunt in uV
};

struct ina219_settings {
    // none
};
