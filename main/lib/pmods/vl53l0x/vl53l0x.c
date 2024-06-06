///
/// @file VL53L0X.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-07-17
///
/// @copyright Copyright (c) 2022
///
///
#include "vl53l0x.h"
#include "vl53l0x_driver.h"
#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"

PMOD_SETUP_API(vl53l0x)
{
    ((struct vl53l0x_stm *)inst->cfg.self)->slave = inst->cfg.com;
    uint8_t rev_id = bi2c_read_reg_u8(inst->cfg.com, IDENTIFICATION_REVISION_ID);
    (void)rev_id;
    if (VL53L0X_init((struct vl53l0x_stm *)inst->cfg.self, true)) {
        VL53L0X_startContinuous((struct vl53l0x_stm *)inst->cfg.self, 0);
        //
        return 0;
    } else {
        return -EIO;
    }
}

PMOD_POLL_API(vl53l0x)
{
    struct vl53l0x_sample_value sample;
    sample.proximity = VL53L0X_readRangeContinuousMillimeters((struct vl53l0x_stm *)inst->cfg.self);
    if (sample.proximity != 0xFFFF) {
        on_ready(&sample, sizeof(sample), false);
        return 0;
    }
    return -ENODEV;
}

PMOD_EXIT_API(vl53l0x)
{
    return 0;
}

const struct bi2c_slave vl53l0x_com_cfg = {
    .id = BI2C_0,
    .addr = 0b0101001, // ePMOD_I2C_ADDR_SI7006,
};

struct vl53l0x_stm vl53l0x_self_cfg;

PMOD_INST_DEFINE(vl53l0x);
