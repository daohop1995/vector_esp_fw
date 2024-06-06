///
/// @file iadc.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-05-07
///
/// @copyright Copyright (c) 2022
///
///
#pragma once

#include "hal/bi2c/bi2c.h"

/**
 * @brief
 *
 * @param addr
 * @return true
 * @return false
 */
extern int iadc_setup(uint8_t addr, void *settings, uint16_t settings_len);

/**
 * @brief
 *
 * @param addr
 * @param buf
 * @param buf_len
 */
extern int iadc_loop(bool push_sample, bool send_sample)(uint8_t addr, uint8_t **buf, uint16_t *buf_len);