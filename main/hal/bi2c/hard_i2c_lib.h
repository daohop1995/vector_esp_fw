///
/// @file hard_i2c_lib.h
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

int hard_i2c_lib_init(const struct bi2c_inst *inst);

int hard_i2c_lib_write_bytes(const struct bi2c_inst *inst,
                             const struct bi2c_slave *slave,
                             const uint8_t *cmd, uint16_t cmd_len,
                             const uint8_t *data, uint16_t data_len);
                             
int hard_i2c_lib_read_bytes(const struct bi2c_inst *inst,
                            const struct bi2c_slave *slave,
                            const uint8_t *cmd, uint16_t cmd_len,
                            uint8_t *buf, uint16_t buf_len);
