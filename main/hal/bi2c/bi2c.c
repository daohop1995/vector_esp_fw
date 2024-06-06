
///
/// @file bi2c.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-12-09
///
/// @copyright Copyright (c) 2023
///
///
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "lp_core_i2c.h"
#include "FreeRTOSConfig.h"
#include "bi2c.h"
#include "soft_i2c_lib.h"
#include "hard_i2c_lib.h"

static const char *TAG = "bi2c";

static const i2c_config_t _hard_i2c_cfg[] = {{
    .mode = I2C_MODE_MASTER,
    .scl_io_num = GPIO_NUM_7,
    .sda_io_num = GPIO_NUM_6,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = 100000,
}};

static const struct soft_i2c_cfg _soft_i2c_cfg[] = {{
    .sda_io_num = GPIO_NUM_8,
    .scl_io_num = GPIO_NUM_9,
    .us = 4,
    .stretch_us = 500,
}};

static const struct bi2c_inst _inst[] = {
    [BI2C_0] = {
        .is_soft = false,
        .id = I2C_NUM_0,
        .cfg = &_hard_i2c_cfg[0],
    },
    [BI2C_1] = {
        .is_soft = true,
        .id = LP_I2C_NUM_0,
        .cfg = &_soft_i2c_cfg[0],
    },
};

int bi2c_init(enum bi2c_id id)
{
    return (_inst[id].is_soft)
               ? soft_i2c_lib_init(&_inst[id])
               : hard_i2c_lib_init(&_inst[id]);
}

int bi2c_write_bytes(const struct bi2c_slave *slave,
                     const uint8_t *cmd, uint16_t cmd_len,
                     const uint8_t *data, uint16_t data_len)
{
    return (_inst[slave->id].is_soft)
               ? soft_i2c_lib_write_bytes(&_inst[slave->id], slave, cmd, cmd_len, data, data_len)
               : hard_i2c_lib_write_bytes(&_inst[slave->id], slave, cmd, cmd_len, data, data_len);
}

int bi2c_read_bytes(const struct bi2c_slave *slave,
                    const uint8_t *cmd, uint16_t cmd_len,
                    uint8_t *buf, uint16_t buf_len)
{
    return (_inst[slave->id].is_soft)
               ? soft_i2c_lib_read_bytes(&_inst[slave->id], slave, cmd, cmd_len, buf, buf_len)
               : hard_i2c_lib_read_bytes(&_inst[slave->id], slave, cmd, cmd_len, buf, buf_len);
}

uint8_t bi2c_read_byte(const struct bi2c_slave *slave, uint8_t *addr, uint8_t addr_len)
{
    uint8_t value = 0;
    bi2c_read_bytes(slave, addr, addr_len, &value, 1);
    return value;
}

int bi2c_write_byte(const struct bi2c_slave *slave, uint8_t *addr, uint8_t addr_len, uint8_t value)
{
    return bi2c_write_bytes(slave, addr, addr_len, &value, 1);
}

int bi2c_write_reg_u8(const struct bi2c_slave *slave, uint8_t reg_addr, uint8_t val)
{
    return bi2c_write_byte(slave, &reg_addr, 1, val);
}

int bi2c_write_reg_u16(const struct bi2c_slave *slave, uint8_t reg_addr, uint16_t val, bool msb_first)
{
    if (msb_first) {
        val = __bswap16(val);
    }
    return bi2c_write_bytes(slave, &reg_addr, 1, (uint8_t *)&val, 2);
}

int bi2c_write_reg_u32(const struct bi2c_slave *slave, uint8_t reg_addr, uint32_t val, bool msb_first)
{
    if (msb_first) {
        val = __bswap32(val);
    }
    return bi2c_write_bytes(slave, &reg_addr, 1, (uint8_t *)&val, 4);
}

int bi2c_write_reg_array(const struct bi2c_slave *slave, uint8_t reg_addr, uint8_t *data, uint8_t data_len)
{
    return bi2c_write_bytes(slave, &reg_addr, 1, data, data_len);
}

uint8_t bi2c_read_reg_u8(const struct bi2c_slave *slave, uint8_t reg_addr)
{
    return bi2c_read_byte(slave, &reg_addr, 1);
}

uint16_t bi2c_read_reg_u16(const struct bi2c_slave *slave, uint8_t reg_addr, bool msb_first)
{
    uint16_t val = 0;
    bi2c_read_bytes(slave, &reg_addr, 1, (uint8_t *)&val, 2);
    if (msb_first) {
        val = __bswap16(val);
    }
    return val;
}

uint32_t bi2c_read_reg_u32(const struct bi2c_slave *slave, uint8_t reg_addr, bool msb_first)
{
    uint32_t val = 0;
    bi2c_read_bytes(slave, &reg_addr, 1, (uint8_t *)&val, 4);
    if (msb_first) {
        val = __bswap32(val);
    }
    return val;
}

int bi2c_read_reg_array(const struct bi2c_slave *slave, uint8_t reg_addr, uint8_t *buf, uint8_t buf_len)
{
    return bi2c_read_bytes(slave, &reg_addr, 1, buf, buf_len);
}

int bi2c_write_reg_settings(const struct bi2c_slave *slave, const struct bi2c_reg_setting *settings, uint8_t settings_cnt)
{
    int ret = 0;
    while ((settings_cnt > 0) && (ret == 0)) {
        ret = bi2c_write_reg_u8(slave, settings->reg, settings->value);
        settings_cnt--;
        settings++;
    }
    return ret;
}
