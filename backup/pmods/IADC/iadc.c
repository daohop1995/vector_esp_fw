///
/// @file iadc.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-05-07
///
/// @copyright Copyright (c) 2022
///
///
#include "iadc.h"

static uint16_t adc_val_buf[8];

int iadc_setup(uint8_t addr, void *settings, uint16_t settings_len)
{
    return true;
}

int iadc_loop(bool push_sample, bool send_sample)(uint8_t addr, uint8_t **buf, uint16_t *buf_len)
{
    for (int i = sizeof(adc_val_buf) / sizeof(adc_val_buf[0]); i > 0;) {
        i--;
        adc_val_buf[i] = ADC_GetResult16(0u);
    }
    *buf_len = sizeof(adc_val_buf);
    *buf = (uint8_t *)adc_val_buf;
	return true;
}