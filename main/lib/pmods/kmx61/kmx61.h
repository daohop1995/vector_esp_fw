///
/// @file kmx61.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-04-24
///
/// @copyright Copyright (c) 2022
///
///
#pragma once

#include <stdint.h>
#include <stdbool.h>

struct kmx61_sample_value {
    int16_t xAcc;
    int16_t yAcc;
    int16_t zAcc;
    int16_t temp;
    int16_t xMag;
    int16_t yMag;
    int16_t zMag;
};

struct kmx61_settings {};
