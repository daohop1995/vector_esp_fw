///
/// @file bble.h
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

typedef void (*on_bble_data_write_f)(const uint8_t *data, uint16_t data_len);

/// @brief
///
/// @param
extern int bble_init(on_bble_data_write_f on_data);

/// @brief
/// @param p_data
/// @param data_len
extern int bble_send_data(const uint8_t *data, uint16_t data_len);
