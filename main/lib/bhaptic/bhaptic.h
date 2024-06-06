///
/// @file bhaptic.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-05-07
///
/// @copyright Copyright (c) 2024
///
///
#pragma once

#include <stdint.h>
#include <utils/util_macro.h>

struct bhaptic_wave {
    uint8_t amp;
    uint8_t freq;
    uint8_t cycles;
    uint8_t envelop;
};

extern const struct bhaptic_wave bhaptic_wave_1[4];
extern const struct bhaptic_wave bhaptic_wave_2[6];
extern const struct bhaptic_wave bhaptic_wave_3[2];
extern const struct bhaptic_wave bhaptic_wave_4[1];

int bhaptic_play_wave(const struct bhaptic_wave *waves, uint8_t wave_counts);

int bhaptic_analog_mode();
