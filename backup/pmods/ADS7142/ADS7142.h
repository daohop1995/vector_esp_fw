/**
 * @file ADS7142.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include "hal/bi2c/bi2c.h"

#define ADS7142_ADDRESS (0x18 << 1) // 8 bit format

// Typedefs
typedef enum ads7142_opcode_e
{
    ADS7142_SINGLE_READ = 0b00010000,
    ADS7142_SINGLE_WRITE = 0b00001000,
    ADS7142_SET_BIT = 0b00011000,
    ADS7142_CLEAR_BIT = 0b00100000,
    ADS7142_MULTI_READ = 0b00110000,
    ADS7142_MULTI_WRITE = 0b00101000,
} ads7142_opcode_e;

typedef enum ads7142_register_e
{
    WKEY = 0x17,
    DEVICE_RESET = 0x14,
    OFFSET_CAL = 0x15,
    OPMODE_SEL = 0x1C,
    OPMODE_I2CMODE_STATUS = 0x00,
    CHANNEL_INPUT_CFG = 0x24,
    AUTO_SEQ_CHEN = 0x20,
    START_SEQUENCE = 0x1E,
    ABORT_SEQUENCE = 0x1F,
    SEQUENCE_STATUS = 0x04,
    OSC_SEL = 0x18,
    nCLK_SEL = 0x19,
    DATA_BUFFER_OPMODE = 0x2C,
    DOUT_FORMAT_CFG = 0x28,
    DATA_BUFFER_STATUS = 0x01,
    ACC_EN = 0x30,
    ACC_CH0_LSB = 0x08,
    ACC_CH0_MSB = 0x09,
    ACC_CH1_LSB = 0xA,
    ACC_CH1_MSB = 0xB,
    ACCUMULATOR_STATUS = 0x02,
    ALERT_DWC_EN = 0x37,
    ALERT_CHEN = 0x34,
    DWC_HTH_CH0_MSB = 0x39,
    DWC_HTH_CH0_LSB = 0x38,
    DWC_LTH_CH0_MSB = 0x3B,
    DWC_LTH_CH0_LSB = 0x3B,
    DWC_HYS_CH0 = 0x40,
    DWC_HTH_CH1_MSB = 0x3D,
    DWC_HTH_CH1_LSB = 0x3C,
    DWC_LTH_CH1_MSB = 0x3F,
    DWC_LTH_CH1_LSB = 0x3E,
    DWC_HYS_CH1 = 0x41,
    PRE_ALT_MAX_EVENT_COUNT = 0x36,
    ALERT_TRIG_CHID = 0x03,
    ALERT_LOW_FLAGS = 0x0C,
    ALERT_HIGH_FLAGS = 0x0E
} ads7142_register_t;

typedef enum ads7142_w_opmode_e
{
    ADS7142_MANUAL_MODE = 0,
    ADS7142_MANUAL_MODE_AUTOSEQ = 4,
    ADS7142_AUTO_MODE_AUTODEQ = 6,
    ADS7142_HIGH_PRES_MODE = 7,
} ads7142_w_opmode_t;

typedef enum ads7142_r_opmode_e
{
    ADS7142_DEV_MANUAL_MODE = 0,
    ADS7142_DEV_AUTO_MODE_AUTODEQ = 2,
    ADS7142_DEV_HIGH_PRES_MODE = 3,
} ads7142_r_opmode_t;

typedef enum ads7142_input_ch_e
{
    ADS7142_2CH_SINGLE_END = 0,
    ADS7142_1CH_SINGLE_END_REM_GND = 1,
    ADS7142_2CH_PSEUDO_DIFF = 2,
    ADS7142_2CH_SINGLE_END_1 = 3
} ads7142_input_ch_t;

typedef enum ads7142_seq_stat_e
{
    ADS7142_AUTOSEQ_OFF_OK = 0,
    ADS7142_AUTOSEQ_ON_OK = 1,
    ADS7142_AUTOSEQ_ON_ERROR = 3
} ads7142_seq_stat_t;

typedef enum ads7142_startstop_ctrl_e
{
    ADS7142_STOP_BURST_MODE = 0,
    ADS7142_START_BURST_MODE = 1,
    ADS7142_PRE_AL_DATA_MODE = 4,
    ADS7142_POST_AL_DATA_MODE = 6
} ads7142_startstop_ctrl_t;

typedef enum ads7142_dout_format_e
{
    ADS7142_12B_4B_ZEROPAD = 0,
    ADS7142_12B_3B_CHID = 1,
    ADS7142_12B_3B_CHID_1B_DVALID = 2,
    ADS7142_12B_4B_ZEROPAD_1 = 3
} ads7142_dout_format_t;

/**
 * @brief
 *
 * @param addr
 * @return true
 * @return false
 */
extern int ads7142_setup(uint8_t addr, void *settings, uint16_t settings_len);

/**
 * @brief
 *
 * @param addr
 * @param buf
 * @param buf_len
 */
extern int ads7142_loop(bool push_sample, bool send_sample)(uint8_t addr, uint8_t **buf, uint16_t *buf_len);