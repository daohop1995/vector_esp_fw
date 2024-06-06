///
/// @file soft_i2c_lib.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-02-14
///
/// @copyright Copyright (c) 2024
///
///
#include "soft_i2c_lib.h"
#include <errno.h>

int soft_i2c_lib_init(const struct bi2c_inst *inst)
{
    const struct soft_i2c_cfg *cfg = inst->cfg;
    // Set I2C pins low in setup. With pinMode(OUTPUT), line goes low
    //   With pinMode(INPUT), then pull-up resistor pulls line high
    digitalWrite(scl_io_num, LOW);
    digitalWrite(sda_io_num, LOW);
    pinPullup(scl_io_num);
    pinPullup(sda_io_num);
    pinMode(scl_io_num, INPUT);
    pinMode(sda_io_num, INPUT);
    return 0;
}

static void sclHi(const struct soft_i2c_cfg *cfg)
{

    // I2C pull-up resistor pulls SCL high in INPUT (Hi-Z) mode
    pinMode(scl_io_num, INPUT);
    delay_us(cfg->us);

    // Check to make sure SCL pin has actually gone high before returning
    // Device may be pulling SCL low to delay transfer (clock stretching)
    // uint32_t stretch_us = cfg->stretch_us;
    // do {
    //     if (digitalRead(scl_io_num) == HIGH) {
    //         delay_us(cfg->us);
    //         return;
    //     }
    //     stretch_us--;
    // } while (stretch_us);
    // cfg->_stretch_timeout_error = 1;
}

static void sclLo(const struct soft_i2c_cfg *cfg)
{
    pinMode(scl_io_num, OUTPUT); // scl_io_num set LOW in constructor
    delay_us(cfg->us - 3);
}

static void sdaHi(const struct soft_i2c_cfg *cfg)
{
    pinMode(sda_io_num, INPUT); // I2C pull-up resistor pulls signal high
}

static void sdaLo(const struct soft_i2c_cfg *cfg)
{
    pinMode(sda_io_num, OUTPUT); // sda_io_num set LOW in constructor
}

static void startBit(const struct soft_i2c_cfg *cfg)
{
    // Assume SDA already HIGH
    sclHi(cfg);
    sdaLo(cfg);
    sclLo(cfg);
}

static uint8_t checkAckBit(const struct soft_i2c_cfg *cfg)
{
    // Can also be used by controller to send NACK after last byte is read from device
    uint8_t ack;
    // sdaHi(cfg); // Release data line. This will cause a NACK from controller when reading bytes.
    sclHi(cfg);
    ack = digitalRead(sda_io_num);
    sclLo(cfg);
    return ack;
}

static void writeAck(const struct soft_i2c_cfg *cfg)
{
    // Used by controller to ACK to device bewteen multi-byte reads
    sdaLo(cfg);
    sclHi(cfg);
    sclLo(cfg);
    sdaHi(cfg); // Release the data line
}

static void stopBit(const struct soft_i2c_cfg *cfg)
{
    // Assume SCK is already LOW (from ack or data write)
    sdaLo(cfg);
    sclHi(cfg);
    sdaHi(cfg);
}

static void writeByte(const struct soft_i2c_cfg *cfg, uint8_t data)
{
    uint8_t mask = 0x80;
    while (mask) {
        if (data & mask) {
            sdaHi(cfg);
        } else {
            sdaLo(cfg);
        }
        sclHi(cfg);
        sclLo(cfg);
        mask >>= 1;
    }
    sdaHi(cfg); // Release the data line for ACK signal from device
}

static void writeAddress(const struct soft_i2c_cfg *cfg, uint8_t dev_addr, bool read)
{
    writeByte(cfg, dev_addr << 1 | read);
}

static uint8_t read1Byte(const struct soft_i2c_cfg *cfg)
{
    uint8_t value = 0;
    for (int i = 0; i < 8; i++) {
        sclHi(cfg);
        value <<= 1;
        value |= digitalRead(sda_io_num);
        sclLo(cfg);
    }
    return value;
}

int soft_i2c_read_write_start(const struct soft_i2c_cfg *cfg, uint8_t addr, bool read)
{
    startBit(cfg);
    writeAddress(cfg, addr, read); // 0 == Write bit
    if (checkAckBit(cfg)) {
        stopBit(cfg);
        return -EADDRNOTAVAIL;
    }
    return 0;
}

int soft_i2c_lib_write_bytes(const struct bi2c_inst *inst,
                             const struct bi2c_slave *slave,
                             const uint8_t *cmd, uint16_t cmd_len,
                             const uint8_t *data, uint16_t data_len)
{
    int err = 0;
    const struct soft_i2c_cfg *cfg = inst->cfg;
    err = soft_i2c_read_write_start(cfg, slave->addr, false);
    if (err)
        return err;
    while ((err == 0) && (cmd) && (cmd_len)) {
        writeByte(cfg, *cmd);
        if (checkAckBit(cfg)) {
            err = -EIO;
            break;
        }
        cmd++;
        cmd_len--;
    }
    switch (slave->write_mode) {
    case BI2C_STOP_START:
        stopBit(cfg);
    case BI2C_RESTART:
        err = soft_i2c_read_write_start(cfg, slave->addr, false);
        break;
    default:
        break;
    }
    if (err)
        return err;
    while ((err == 0) && (data) && (data_len)) {
        writeByte(cfg, *data);
        if (checkAckBit(cfg)) {
            err = -EIO;
            break;
        }
        data++;
        data_len--;
    }
    stopBit(cfg);
    return err;
}

int soft_i2c_lib_read_bytes(const struct bi2c_inst *inst,
                            const struct bi2c_slave *slave,
                            const uint8_t *cmd, uint16_t cmd_len,
                            uint8_t *buf, uint16_t buf_len)
{
    int err = 0;
    const struct soft_i2c_cfg *cfg = inst->cfg;
    err = soft_i2c_read_write_start(cfg, slave->addr, false);
    if (err)
        return err;
    while ((err == 0) && (cmd) && (cmd_len)) {
        writeByte(cfg, *cmd);
        if (checkAckBit(cfg)) {
            err = -EIO;
            break;
        }
        cmd++;
        cmd_len--;
    }
    switch (slave->read_mode) {
    case BI2C_STOP_START:
        stopBit(cfg);
    case BI2C_RESTART:
        err = soft_i2c_read_write_start(cfg, slave->addr, true);
        break;
    default:
        break;
    }
    if (err)
        return err;
    while ((err == 0) && (buf) && (buf_len)) {
        *buf = read1Byte(cfg);
        if (buf_len > 1) {
            writeAck(cfg);
        } else {              // Last byte needs a NACK
            sdaHi(cfg);       // Release the data line
            checkAckBit(cfg); // Controller needs to send NACK when done reading data
        }
        buf++;
        buf_len--;
    }
    stopBit(cfg);
    return err;
}
