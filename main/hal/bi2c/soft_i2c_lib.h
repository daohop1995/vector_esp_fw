///
/// @file soft_i2c_lib.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-02-14
///
/// @copyright Copyright (c) 2024
///
///

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "bi2c.h"
#include "soft_i2c_lib_def.h"

struct soft_i2c_cfg {
    uint8_t sda_io_num;
    uint8_t scl_io_num;
    uint16_t us;
    uint32_t stretch_us;
};

int soft_i2c_lib_init(const struct bi2c_inst *inst);

int soft_i2c_lib_write_bytes(const struct bi2c_inst *inst,
                             const struct bi2c_slave *slave,
                             const uint8_t *cmd, uint16_t cmd_len,
                             const uint8_t *data, uint16_t data_len);

int soft_i2c_lib_read_bytes(const struct bi2c_inst *inst,
                            const struct bi2c_slave *slave,
                            const uint8_t *cmd, uint16_t cmd_len,
                            uint8_t *buf, uint16_t buf_len);
