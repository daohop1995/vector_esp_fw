/// 
/// @file rgb_led.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief 
/// @version 0.1
/// @date 2023-06-17
/// 
/// @copyright Copyright (c) 2023
/// 
/// 
#pragma once

#include "main.h"

/// @brief 
/// 
/// @param  
extern int brgb_init(void);

/// @brief 
///
/// @param r 
/// @param g 
/// @param b 
extern int brgb_set(uint8_t led_num, uint8_t r, uint8_t g, uint8_t b);
