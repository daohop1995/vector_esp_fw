///
/// @file baudio_codec.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-02-24
///
/// @copyright Copyright (c) 2024
///
///

#pragma once

#include <stdint.h>
#include "hal/bi2c/bi2c.h"
#include "hal/bi2s/bi2s.h"

enum baudio_outputs {
    OUTPUT_NONE,
    OUTPUT_1_HP_MONO,
    OUTPUT_1_SPK_MONO,
    OUTPUT_1_HP_STEREO,
    OUTPUT_1_SPK_STEREO,
};

enum baudio_inputs {
    INPUT_NONE,
    INPUT_MIC_1,
};

enum baudio_freq {
    BAUDIO_SR_8_KHz = 8000,
    BAUDIO_SR_16_KHz = 16000,
    BAUDIO_SR_22p05_KHz = 22050,
    BAUDIO_SR_32_KHz = 32000,
    BAUDIO_SR_44p1_KHz = 44100,
    BAUDIO_SR_48_KHz = 48000,
};

struct baudio_cfg {
    union {
        struct bi2c_slave *i2c;
    } com;
    uint32_t reset_gpio_num;
    uint32_t freq;
};

struct baudio_inst {
    const struct baudio_cfg *cfg;
    const struct baudio_codec_api *api;
    uint32_t codec_freq;
    enum baudio_outputs cur_output;
    enum baudio_inputs cur_input;
};

struct baudio_codec_api {
    int (*init)(struct baudio_inst *inst);
    int (*deinit)(struct baudio_inst *inst);
    int (*reset)(struct baudio_inst *inst);
    //
    int (*select_output)(struct baudio_inst *inst, enum baudio_outputs output);
    int (*mute_output)(struct baudio_inst *inst, bool mute);
    int (*set_output_sample_rate)(struct baudio_inst *inst, uint32_t sample_rate);
    int (*set_output_volume)(struct baudio_inst *inst, int16_t volume);
    int (*set_output_gain)(struct baudio_inst *inst, int8_t gain);
    //
    int (*select_input)(struct baudio_inst *inst, enum baudio_inputs input);
    int (*set_input_mute)(struct baudio_inst *inst, uint8_t mute_state);
    int (*set_input_sample_rate)(struct baudio_inst *inst, uint32_t sample_rate);
    //
    int (*beep)(struct baudio_inst *inst, uint16_t freq, uint16_t ms);
    //
    uint16_t (*get_output_pending)(struct baudio_inst *inst);
    uint16_t (*get_input_pending)(struct baudio_inst *inst);
    //
    int (*start_transfer)(struct baudio_inst *inst, uint16_t *tx_data, uint16_t *rx_data, uint16_t size);
};

#define BAUDIO_API_DECLARE(name) name##_baudio_apis
#define BAUDIO_API_DEFINE(name)                                  \
    const struct baudio_codec_api BAUDIO_API_DECLARE(name) = {   \
        .init = name##_init,                                     \
        .deinit = name##_deinit,                                 \
        .reset = name##_reset,                                   \
        .select_output = name##_select_output,                   \
        .mute_output = name##_mute_output,                       \
        .set_output_volume = name##_set_output_volume,           \
        .set_output_gain = name##_set_output_gain,               \
        .set_output_sample_rate = name##_set_output_sample_rate, \
        .select_input = name##_select_input,                     \
        .set_input_mute = name##_set_input_mute,                 \
        .set_input_sample_rate = name##_set_input_sample_rate,   \
        .beep = name##_beep,                                     \
        .get_output_pending = name##_get_output_pending,         \
        .get_input_pending = name##_get_input_pending,           \
        .start_transfer = name##_start_transfer,                 \
    }

extern const struct baudio_codec_api BAUDIO_API_DECLARE(tlv320aic3120);
void tlv320_clock_cfg(struct baudio_inst *inst);
void tlv320_speaker_config(struct baudio_inst *inst);