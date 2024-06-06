///
/// @file adxl346.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-05-10
///
/// @copyright Copyright (c) 2022
///
///
#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"
#include "adxl346.h"
#include "esp_log.h"

static const char *TAG = "adxl";

PMOD_SETUP_API(adxl346)
{
    if (bi2c_read_reg_u8(inst->cfg.com, ADXL346_DEVID_ADDR) == ADXL346_DEVID_VALUE) {
        bi2c_write_reg_u8(inst->cfg.com, ADXL346_BW_RATE_ADDR, ADXL346_BW_RATE_RATE(6));
        bi2c_write_reg_u8(inst->cfg.com, ADXL346_DATA_FORMAT_ADDR, ADXL346_DATA_FORMAT_RANGE_PM_16g);
        bi2c_write_reg_u8(inst->cfg.com, ADXL346_POWER_CTL_ADDR, ADXL346_POWER_CTL_MEASURE);
        return 0;
    } else {
        return -EFAULT;
    }
}

PMOD_POLL_API(adxl346)
{
    int err;
    struct adxl346_sample_value sample;
    err = bi2c_read_bytes(inst->cfg.com, (uint8_t[1]){ADXL346_DATAX0_ADDR}, 1, (uint8_t *)&sample, sizeof(sample));
    if (err == 0) {
        ESP_LOGE(TAG, "ADXL x:%d, y:%d, z:%d", sample.x, sample.y, sample.z);
        on_ready(&sample, sizeof(sample), false);
    }
    return err;
}

PMOD_EXIT_API(adxl346)
{
    return 0;
}

const struct bi2c_slave adxl346_com_cfg = {
    .id = BI2C_0,
    .addr = ePMOD_I2C_ADDR_ADXL346,
    .read_mode = BI2C_RESTART,
    .write_mode = BI2C_NO_RESTART,
};

const uint8_t adxl346_self_cfg;

PMOD_INST_DEFINE(adxl346);
