///
/// @file bhaptic.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-05-07
///
/// @copyright Copyright (c) 2024
///
///
#include "bhaptic.h"
#include "hal/bi2c/bi2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "hPZO";

static const struct bi2c_slave haptic_slave = {
    .id = BI2C_1,
    .addr = 0x59,
    .read_mode = BI2C_RESTART,
    .write_mode = BI2C_NO_RESTART,
};

const struct bhaptic_wave bhaptic_wave_1[4] = {
    {255, 0x15, 50, 0x09},
    {255, 0x17, 50, 0x09},
    {255, 0x19, 50, 0x09},
    {255, 0x1B, 50, 0x09},
};

const struct bhaptic_wave bhaptic_wave_2[6] = {
    {255, 0x15, 50, 0x09},
    {255, 0x17, 50, 0x09},
    {255, 0x19, 50, 0x09},
    {255, 0x1B, 50, 0x09},
    {255, 0x1D, 50, 0x09},
    {255, 0x1F, 50, 0x09},
};

const struct bhaptic_wave bhaptic_wave_3[2] = {
    {255, 0x15, 50, 0x09},
    {255, 0x17, 50, 0x09},
};

const struct bhaptic_wave bhaptic_wave_4[1] = {
    {255, 0x15, 50, 0x09},
};

int bhaptic_play_wave(const struct bhaptic_wave *waves, uint8_t wave_counts)
{
	int ret = 0;
    const struct bi2c_reg_setting reg_settings_1[] = {
        // control
        {0x02, 0x00}, // Take device out of standby mode
        {0x01, 0x03}, // Set Gain 0-3 (0x00-0x03 25v-100v)
        {0x03, 0x01}, // Set sequencer to play WaveForm ID #1
        {0x04, 0x00}, // End of sequence
        // header
        {0xFF, 0x01}, // Set memory to page 1
        {0x00, 0x05}, // Header size –1
        {0x01, 0x80}, // Start address upper byte (page), also indicates Mode 3
        {0x02, 0x06}, // Start address lower byte (in page address)
        {0x03, 0x00}, // Stop address upper byte
        {0x05, 0x01}, // Repeat count, play WaveForm once
    };
    struct bi2c_reg_setting reg_settings_2[] = {
        {0x04, 0},    // Stop address Lower byte
    };
    struct bi2c_reg_setting reg_settings_3[] = {
        {0xFF, 0x00}, // Set page register to control space
        {0x02, 0x01}, // Set GO bit (execute WaveForm sequence)ss
    };

    ESP_LOGD(TAG, "Playing Waveform");
    reg_settings_2[0].value = 0x06 + (wave_counts * 4) - 1;
    ret = bi2c_write_reg_settings(&haptic_slave, reg_settings_1, ARRAY_LEN(reg_settings_1));
    ret = bi2c_write_reg_settings(&haptic_slave, reg_settings_2, ARRAY_LEN(reg_settings_2));
    ret = bi2c_write_reg_array(&haptic_slave, 0x06, (uint8_t *)(waves), wave_counts * 4);
    ret = bi2c_write_reg_settings(&haptic_slave, reg_settings_3, ARRAY_LEN(reg_settings_3));
	return ret;
}

int bhaptic_analog_mode()
{
    ESP_LOGD(TAG, "switching into analog");
    const struct bi2c_reg_setting reg_settings[] = {
        // control
        {0x02, 0x00}, // Take device out of standby mode
        {0x02, 0x02}, // Set EN_OVERRIDE bit = boost and amplifier active
        {0x01, 0x07}, // Set to analog input + Gain 0-3 (0x04-0x07 25v-100v)
    };
    int ret = bi2c_write_reg_settings(&haptic_slave, reg_settings, ARRAY_LEN(reg_settings));
	return ret;
}
