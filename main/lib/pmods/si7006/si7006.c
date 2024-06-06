/**
 * @file si7006.c
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-24
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"
#include "si7006.h"

PMOD_SETUP_API(si7006)
{
    int ret;
    if ((ret = bi2c_read_byte(inst->cfg.com, (uint8_t[]){CMD_READ_RH_T_USER_REG1}, 1)) == 0b00111010) {
        return 0;
    } else {
        return -EFAULT;
    }
}

PMOD_POLL_API(si7006)
{
    int err;
    struct si7006_sample_value sample;
    uint8_t temp_buf[2] = {0};
    //
    int32_t h = 0;
    //!
    if ((err = bi2c_read_bytes(inst->cfg.com, (uint8_t[]){CMD_HOLD_MASTER_MODE_H}, 1, temp_buf, 2)) == 0) {
        h = (temp_buf[0] << 8) | temp_buf[1];
        h *= 125;
        h = h >> 16;
        h -= 6;
        sample.humidity = h;
        if ((err = bi2c_read_bytes(inst->cfg.com, (uint8_t[]){CMD_READ_TEMP_PREV_VALUE}, 1, temp_buf, 2)) == 0) {
            h = (temp_buf[0] << 8) | temp_buf[1];
            h *= 17522;
            h = h >> 16;
            h -= 4685;
            sample.temperature = h;
            on_ready(&sample, sizeof(sample), false);
        }
    }
    return err;
}

PMOD_EXIT_API(si7006)
{
    return 0;
}

const struct bi2c_slave si7006_com_cfg = {
    .id = BI2C_0,
    .addr = ePMOD_I2C_ADDR_SI7006,
};

const uint8_t si7006_self_cfg;

PMOD_INST_DEFINE(si7006);
