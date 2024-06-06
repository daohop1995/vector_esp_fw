///
/// @file bi2c.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-12-09
///
/// @copyright Copyright (c) 2023
///
///

#pragma once

#include <stdint.h>
#include <stdbool.h>

enum bi2c_id {
    BI2C_0,
    BI2C_1,
};

enum bi2c_slave_restart_mode {
    BI2C_NO_RESTART,
    BI2C_RESTART,
    BI2C_STOP_START,
};

struct bi2c_inst {
    enum bi2c_id id;
    bool is_soft;
    void *cfg;
};

struct bi2c_slave {
    enum bi2c_id id;
    uint8_t addr;
    enum bi2c_slave_restart_mode read_mode;
    enum bi2c_slave_restart_mode write_mode;
};

struct bi2c_reg_setting {
    uint8_t reg;
    uint8_t value;
};

///
/// @brief
///
///
/// @return int
///
extern int bi2c_init(enum bi2c_id id);

///
/// @brief
///
///
/// @param slave
/// @param cmd
/// @param cmd_len
/// @param data
/// @param data_len
/// @return int
///
extern int bi2c_write_bytes(const struct bi2c_slave *slave,
                            const uint8_t *cmd, uint16_t cmd_len,
                            const uint8_t *data, uint16_t data_len);

///
/// @brief
///
///
/// @param slave
/// @param cmd
/// @param cmd_len
/// @param buf
/// @param buf_len
/// @return int
///
extern int bi2c_read_bytes(const struct bi2c_slave *slave,
                           const uint8_t *cmd, uint16_t cmd_len,
                           uint8_t *buf, uint16_t buf_len);

///
/// @brief
///
///
/// @param slave
/// @param addr
/// @param addr_len
/// @return uint8_t
///
extern uint8_t bi2c_read_byte(const struct bi2c_slave *slave, uint8_t *addr, uint8_t addr_len);

///
/// @brief
///
///
/// @param slave
/// @param addr
/// @param addr_len
/// @param value
/// @return int
///
extern int bi2c_write_byte(const struct bi2c_slave *slave, uint8_t *addr, uint8_t addr_len, uint8_t value);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @param val
/// @return int
///
extern int bi2c_write_reg_u8(const struct bi2c_slave *slave, uint8_t reg_addr, uint8_t val);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @param val
/// @param msb_first
/// @return int
///
extern int bi2c_write_reg_u16(const struct bi2c_slave *slave, uint8_t reg_addr, uint16_t val, bool msb_first);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @param val
/// @param msb_first
/// @return int
///
extern int bi2c_write_reg_u32(const struct bi2c_slave *slave, uint8_t reg_addr, uint32_t val, bool msb_first);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @param data
/// @param data_len
/// @return int
///
extern int bi2c_write_reg_array(const struct bi2c_slave *slave, uint8_t reg_addr, uint8_t *data, uint8_t data_len);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @return uint8_t
///
extern uint8_t bi2c_read_reg_u8(const struct bi2c_slave *slave, uint8_t reg_addr);

/// @brief
/// @param
/// @param slave
/// @param reg_addr
/// @param msb_first
/// @return
extern uint16_t bi2c_read_reg_u16(const struct bi2c_slave *slave, uint8_t reg_addr, bool msb_first);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @param msb_first
/// @return uint32_t
///
extern uint32_t bi2c_read_reg_u32(const struct bi2c_slave *slave, uint8_t reg_addr, bool msb_first);

///
/// @brief
///
///
/// @param slave
/// @param reg_addr
/// @param buf
/// @param buf_len
/// @return int
///
extern int bi2c_read_reg_array(const struct bi2c_slave *slave, uint8_t reg_addr, uint8_t *buf, uint8_t buf_len);

/// @brief 
/// @param slave 
/// @param settings 
/// @param settings_cnt 
/// @return 
extern int bi2c_write_reg_settings(const struct bi2c_slave *slave, const struct bi2c_reg_setting *settings, uint8_t settings_cnt);
