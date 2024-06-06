/**
 * @file ADS7142.c
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "ADS7142.h"

static uint16_t data;

int ads7142_setup(uint8_t addr, void *settings, uint16_t settings_len) // Address in 8 bit format
{
    bi2c_write_reg_u8(addr, CHANNEL_INPUT_CFG, ADS7142_2CH_PSEUDO_DIFF);
    return true;
}

int ads7142_loop(bool push_sample, bool send_sample)(uint8_t addr, uint8_t **buf, uint16_t *buf_len)
{
    uint16_t data = bi2c_read_reg_u16(addr, ADS7142_SINGLE_READ, true) >> 4;
    //
    *buf = (uint8_t *)&data;
    *buf_len = sizeof(data);
	return true;
}