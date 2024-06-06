/**
 * @file ina219.c
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"
#include "ina219.h"

PMOD_SETUP_API(ina219)
{
    return 0;
}

PMOD_POLL_API(ina219)
{
    int err;
    uint8_t buffer[2] = {0};
    struct ina219_sample_value sample;
    //
    if ((err = bi2c_read_bytes(inst->cfg.com, (uint8_t[]){INA219_REG_CURRENT}, 1, buffer, 2)) == 0) {
        sample.current = buffer[0];
        sample.current <<= 8;
        sample.current |= buffer[1];
        if ((err = bi2c_read_bytes(inst->cfg.com, (uint8_t[]){INA219_REG_BUSVOLTAGE}, 1, buffer, 2)) == 0) {
            sample.voltage = buffer[0];
            sample.voltage <<= 8;
            sample.voltage |= buffer[1];
            on_ready(&sample, sizeof(sample), false);
        }
    }
    return err;
}

PMOD_EXIT_API(ina219)
{
    return 0;
}

const struct bi2c_slave ina219_com_cfg = {
    .id = BI2C_0,
    .addr = ePMOD_I2C_ADDR_INA219,
    .read_mode = BI2C_RESTART,
    .write_mode = BI2C_NO_RESTART,
};

const uint8_t ina219_self_cfg;

PMOD_INST_DEFINE(ina219);
