///
/// @file kmx61.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-04-24
///
/// @copyright Copyright (c) 2022
///
///

#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"
#include "kmx61.h"

PMOD_SETUP_API(kmx61)
{
    uint8_t who_am_i = bi2c_read_reg_u8(inst->cfg.com, 0x00);
    if (who_am_i == 0x12) {
        bi2c_write_reg_u8(inst->cfg.com, 0x29, 0x00); // select standby register
        bi2c_write_reg_u8(inst->cfg.com, 0x2A, 0x10); // select control register
        bi2c_write_reg_u8(inst->cfg.com, 0x2B, 0x06); // select 50 Hz Motion Detect
        bi2c_write_reg_u8(inst->cfg.com, 0x2C, 0x22); // select 50 Hz Mag & Acc ODR
        return 0;
    } else {
        return -EFAULT;
    }
}

PMOD_POLL_API(kmx61)
{
    struct kmx61_sample_value sample;
    // read and fill sample
    int err = bi2c_read_reg_array(inst->cfg.com, 0x0A, (uint8_t *)&sample, sizeof(sample));
    if (err == 0) {
        // Convert the data to 12-bits
        sample.xAcc >>= 4;
        sample.yAcc >>= 4;
        sample.zAcc >>= 4;
        // Convert the data to 14-bits
        sample.temp >>= 2;
        sample.xMag >>= 2;
        sample.yMag >>= 2;
        sample.zMag >>= 2;
        // convert to signed representation
        if (sample.xAcc > 2047) {
            sample.xAcc -= 4096;
        }
        if (sample.yAcc > 2047) {
            sample.yAcc -= 4096;
        }
        if (sample.zAcc > 2047) {
            sample.zAcc -= 4096;
        }
        if (sample.xMag > 8191) {
            sample.xMag -= 16384;
        }
        if (sample.yMag > 8191) {
            sample.yMag -= 16384;
        }
        if (sample.zMag > 8191) {
            sample.zMag -= 16384;
        }
        on_ready(&sample, sizeof(sample), false);
    }
    return err;
}

PMOD_EXIT_API(kmx61)
{
    return 0;
}

const struct bi2c_slave kmx61_com_cfg = {
    .id = BI2C_0,
    .addr = ePMOD_I2C_ADDR_KMX61,
};

const uint8_t kmx61_self_cfg;

PMOD_INST_DEFINE(kmx61);
