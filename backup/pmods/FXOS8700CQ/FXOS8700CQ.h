/**
 * @file FXOS8700CQ.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-24
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include "hal/bi2c/bi2c.h"

// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR 0x1E // with pins SA0=0, SA1=0

// FXOS8700CQ internal register addresses
#define FXOS8700CQ_STATUS 0x00
#define FXOS8700CQ_WHOAMI 0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 0x2A
#define FXOS8700CQ_M_CTRL_REG1 0x5B
#define FXOS8700CQ_M_CTRL_REG2 0x5C
#define FXOS8700CQ_WHOAMI_VAL 0xC7

// number of bytes to be read from the FXOS8700CQ
#define FXOS8700CQ_READ_LEN 13 // status plus 6 channels =

/**
 * @brief
 *
 */
extern int fxos8700cq_setup(uint8_t addr, void *settings, uint16_t settings_len);

/**
 * @brief
 *
 */
extern int fxos8700cq_loop(bool push_sample, bool send_sample);