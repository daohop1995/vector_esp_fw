#include "lib/baudio/baudio_codec.h"
#include "hal/bi2c/bi2c.h"
#include "hal/bi2s/bi2s.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <errno.h>
#include "esp_log.h"
#include <utils/util_macro.h>

static const char *TAG = "tlv320";

/**
 * Values taken from the TVL320AIC3120IRHBT Data Sheet
 * https://www.ti.com/product/TLV320AIC3120/part-details/TLV320AIC3120IRHBT
 */

/**
 * PAGE 0 Defines
 */
#define TVL320_PAGE_0 0x00

#define TVL320_PAGE_0_CONTROL_REG00                                     0x00
#define TVL320_SOFTWARE_RESET_REG01                                     0X01
#define TVL320_OT_FLAG_REG03                                            0X03
#define TVL320_CLOCK_GEN_MUXING_REG04                                   0x04
#define TVL320_PLL_P_AND_R_VALUES_REG05                                 0x05
#define TVL320_PLL_J_VALUE_REG06                                        0x06
#define TVL320_PLL_D_VALUE_MSB_REG07                                    0x07
#define TVL320_PLL_D_VALUE_LSB_REG08                                    0x08
#define TVL320_DAC_NDAC_VAL_REG11                                       0x0B
#define TVL320_DAC_MDAC_VAL_REG12                                       0x0C
#define TVL320_DAC_DOSR_VAL_MSB_REG13                                   0x0D
#define TVL320_DAC_DOSR_VAL_LSB_REG14                                   0x0E
#define TVL320_DAC_IDAC_VAL_REG15                                       0x0F
#define TVL320_DAC_MINIDSP_ENGINE_INTERPOLATION_REG16                   0x10
#define TVL320_ADC_NADC_VAL_REG18                                       0x12
#define TVL320_ADC_MADC_VAL_REG19                                       0x13
#define TVL320_ADC_AOSR_VAL_REG20                                       0x14
#define TVL320_ADC_IADC_VAL_REG21                                       0x15
#define TVL320_ADC_MINIDSP_ENGINE_DECIMATION_REG22                      0x16
#define TVL320_CLKOUT_MUX_REG25                                         0x19
#define TVL320_CLKOUT_M_DIVIDER_VALUE_REG26                             0x1A
#define TVL320_CODEC_INTERFACE_CONTROL_REG27                            0x1B
#define TVL320_DATA_SLOT_OFFSET_PROGRAMMABILITY_REG28                   0x1C
#define TVL320_CODEC_INTERFACE_CONTROL_2_REG29                          0x1D
#define TVL320_BCLK_N_DIVIDER_VALUE_REG30                               0x1E
#define TVL320_CODEC_SECONDARY_INTERFACE_CONTROL_1_REG31                0x1F
#define TVL320_CODEC_SECONDARY_INTERFACE_CONTROL_2_REG32                0x20
#define TVL320_CODEC_SECONDARY_INTERFACE_CONTROL_3_REG33                0x21
#define TVL320_CODEC_I2C_BUS_CONDITION_REG34                            0x22
#define TVL320_ADC_FLAG_REGISTER_REG36                                  0x24
#define TVL320_DAC_FLAG_REGISTER_REG37                                  0x25
#define TVL320_DAC_PGA_FLAG_REGISTER_REG38                              0x26
#define TVL320_OVERFLOW_FLAGS_REG39                                     0x27
#define TVL320_INTERRUPT_FLAGS_DAC_REG44                                0x2C
#define TVL320_INTERRUPT_FLAGS_ADC_REG45                                0x2D
#define TVL320_INTERRUPT_FLAGS_DAC_2_REG46                              0x2E
#define TVL320_INTERRUPT_FLAGS_ADC_2_REG47                              0x2F
#define TVL320_INT1_CONTROL_REGISTER_REG48                              0x30
#define TVL320_INT2_CONTROL_REGISTER_REG49                              0x31
#define TVL320_GPIO1_IN_OUT_PIN_CONTROL_REG51                           0x33
#define TVL320_DOUT_OUT_PIN_CONTROL_REG53                               0x35
#define TVL320_DIN_IN_PIN_CONTROL_REG54                                 0x36
#define TVL320_DAC_PROCESSING_BLOCK_MINIDSP_SELECTION_REG60             0x3C
#define TVL320_ADC_PROCESSING_BLOCK_MINIDSP_SELECTION_REG61             0x3D
#define TVL320_PROGRAMMABLE_MINIDSP_INSTRUCTION_MODE_CONTROL_BITS_REG62 0x3E
#define TVL320_DAC_DATA_PATH_SETUP_REG63                                0x3F
#define TVL320_DAC_MUTE_CONTROL_REG64                                   0x40
#define TVL320_DAC_VOLUME_CONTROL_REG65                                 0x41
#define TVL320_HEADSET_DETECTION_REG67                                  0x43
#define TVL320_DRC_CONTROL_1_REG68                                      0x44
#define TVL320_DRC_CONTROL_2_REG69                                      0x45
#define TVL320_DRC_CONTROL_3_REG70                                      0x46
#define TVL320_BEEP_GENERATOR_REG71                                     0x47
#define TVL320_BEEP_LENGTH_MSB_REG73                                    0x49
#define TVL320_BEEP_LENGTH_MIDDLE_BITS_REG74                            0x4A
#define TVL320_BEEP_LENGTH_LSB_REG75                                    0x4B
#define TVL320_BEEP_SIN_MSB_REG76                                       0x4C
#define TVL320_BEEP_SIN_LSB_REG77                                       0x4D
#define TVL320_BEEP_COS_MSB_REG78                                       0x4E
#define TVL320_BEEP_COS_LSB_REG79                                       0x4F
#define TVL320_ADC_DIGITAL_MIC_REG81                                    0x51
#define TVL320_ADC_DIGITAL_VOLUME_CONTROL_FINE_ADJUST_REG82             0x52
#define TVL320_ADC_DIGITAL_VOLUME_CONTROL_COARSE_ADJUST_REG83           0x53
#define TVL320_AGC_CONTROL_1_REG86                                      0x56
#define TVL320_AGC_CONTROL_1_REG87                                      0x57
#define TVL320_AGC_MAXIMUM_GAIN_REG88                                   0x58
#define TVL320_AGC_ATTACK_TIME_REG89                                    0x59
#define TVL320_AGC_DECAY_TIME_REG90                                     0x5A
#define TVL320_AGC_NOISE_DEBOUNCE_REG91                                 0x5B
#define TVL320_AGC_SIGNAL_DEBOUNCE_REG92                                0x5C
#define TVL320_AGC_GAIN_APPLIED_READING_REG93                           0x5D
#define TVL320_ADC_DC_MEASUREMENT_1_REG102                              0x66
#define TVL320_ADC_DC_MEASUREMENT_2_REG103                              0x67
#define TVL320_ADC_DC_MEASUREMENT_OUTPUT1_REG104                        0x68
#define TVL320_ADC_DC_MEASUREMENT_OUTPUT2_REG105                        0x69
#define TVL320_ADC_DC_MEASUREMENT_OUTPUT3_REG106                        0x6A
#define TVL320_VOL_MICSET_PIN_SAR_ADC_VOLUME_CONTROL_REG116             0x74
#define TVL320_VOL_MICDET_PIN_GAIN_REG117                               0x75

/**
 * PAGE 1 Defines
 */

#define TVL320_PAGE_1 0x01

#define TVL320_PAGE_1_CONTROL_REG00                                                        0x00
#define TVL320_HEADPHONE_AND_SPEAKER_AMPLIFIER_ERROR_CONTROL_REG30                         0x1E
#define TVL320_HEADPHONE_DRIVERS_REG31                                                     0X1F
#define TVL320_CLASS_D_SPEAKER_AMP_REG32                                                   0x20
#define TVL320_HP_OUTPUT_DRIVERS_POP_REMOVAL_SETTINGS_REG33                                0x21
#define TVL320_OUTPUT_DRIVER_PGA_RAMP_DOWN_PERIOD_CONTROL_REG34                            0x22
#define TVL320_DAC_OUTPUT_MIXER_ROUTING_REG35                                              0x23
#define TVL320_ANALOG_VOLUME_TO_HPOUT_REG36                                                0x24
#define TVL320_ANALOG_VOLUME_TO_CLASS_D_OUTPUT_DRIVER_REG38                                0x26
#define TVL320_HPOUT_DRIVER_REG40                                                          0x28
#define TVL320_CLASS_D_OUTPUT_DRIVER_DRIVER_REG42                                          0x2A
#define TVL320_HP_DRIVER_CONTROL_REG44                                                     0x2C
#define TVL320_MICBIAS_REG46                                                               0x2E
#define TVL320_DELTA_SIGMA_MONO_ADC_CHANNEL_FINE_GAIN_INPUT_SELECTION_FOR_P_TERMINAL_REG48 0x30
#define TVL320_ADC_INPUT_SELECTION_FOR_M_TERMINAL_REG49                                    0x31
#define TVL320_INPUT_CM_SETTINGS_REG50                                                     0x32

/**
 * PAGE 3 Defines
 */

#define TVL320_PAGE_3 3

#define TVL320_PAGE_3_CONTROL_REG00           0x00
#define TVL320_TIMER_CLOCK_MCLK_DIVIDER_REG16 0x10

/**
 * PAGE 8 Defines
 */

#define TVL320_PAGE_8 8

#define TVL320_PAGE_8_CONTROL_REG00              0x00
#define TVL320_DAC_COEFFICIENT_RAM_CONTROL_REG01 0x01

/**
 * Refer to documentation for:
 * - PAGE 4: ADC Digital Filter Coefficients
 * - PAGE 5: ADC Programmable Coefficients RAM
 * - PAGE 8: DAC Programmable Coefficients RAM Buffer A (1:63)
 * - PAGE 9: DAC Programmable Coefficients RAM Buffer A (65:127)
 * - PAGE 10: DAC Programmable Coefficients RAM Buffer A (129:191)
 * - PAGE 11: DAC Programmable Coefficients RAM Buffer A (193:255)
 * - PAGE 12: DAC Programmable Coefficients RAM Buffer B (1:63)
 * - PAGE 13: DAC Programmable Coefficients RAM Buffer B (65:127)
 * - PAGE 14: DAC Programmable Coefficients RAM Buffer B (129:191)
 * - PAGE 15: DAC Programmable Coefficients RAM Buffer B (193:255)
 * - PAGE 32: ADC DSP Engine Instruction RAM (0:31)
 * - PAGE 33 - 43: ADC DSP Engine Instruction RAM (32:63)
 * - PAGE 64: DAC DSP Engine Instruction RAM (0:31)
 * - PAGE 65 - 95: DAC DSP Engine Instuction RAM (32:63)
 */

static inline int tlv320_read_byte(struct baudio_inst *inst, uint8_t reg)
{
    int err = 0;
    uint8_t val = 0;
    err = bi2c_read_bytes(inst->cfg->com.i2c, &reg, 1, &val, 1);
    if (err) {
        ESP_LOGE(TAG, "Read from %02X failed with %d", reg, err);
    }
    return val;
}

static inline int tlv320_write_byte(struct baudio_inst *inst, uint8_t reg, uint8_t val)
{
    int err = bi2c_write_reg_u8(inst->cfg->com.i2c, reg, val);
    if (err)
        ESP_LOGE(TAG, "Write %02X, at %02X failed with %d", val, reg, err);
    return err;
}

static inline int tlv320_write_byte_check(struct baudio_inst *inst, uint8_t reg, uint8_t val)
{
    int err = tlv320_write_byte(inst, reg, val);
    int val_read = tlv320_read_byte(inst, reg);
    if (val != val_read)
        ESP_LOGE(TAG, "Mismatch in value written (%02x) vs value read (%02x) to register (%02x)", val, val_read, reg);
    return err;
}

static inline int tlv320_write_u16_check(struct baudio_inst *inst, uint8_t reg, uint16_t val)
{
    int err = bi2c_write_bytes(inst->cfg->com.i2c, &reg, 1, (uint8_t *)&val, 2);
    if (err)
        ESP_LOGE(TAG, "Write %04X, at %02X failed with %d", val, reg, err);
    uint16_t val_read;
    err = bi2c_read_bytes(inst->cfg->com.i2c, &reg, 1, (uint8_t *)&val_read, 2);
    if (val != val_read)
        ESP_LOGE(TAG, "Mismatch in value written (%04x) vs value read (%04x) to register (%02x)", val, val_read, reg);
    return err;
}

static inline int tlv320_write_bytes(struct baudio_inst *inst, uint8_t reg, uint8_t *val, uint8_t len)
{
    int err = bi2c_write_reg_array(inst->cfg->com.i2c, reg, val, len);
    if (err)
        ESP_LOGE(TAG, "Write at %02X failed with %d", reg, err);
    return err;
}

static inline int tlv320_poll_for_reg_val(struct baudio_inst *inst, uint8_t reg, uint8_t mask, uint8_t val)
{
    int err = 0;
    do {
        err = tlv320_read_byte(inst, reg);
    } while ((err >= 0) && (err & mask) != val);
    ESP_LOGE(TAG, "Reg:%x, val: %x", reg, err);
    return err;
}

static int tvl320_set_mute(struct baudio_inst *inst, bool mute)
{
    return tlv320_write_byte_check(inst, TVL320_DAC_MUTE_CONTROL_REG64, (mute) ? 0x0C : 0x04);
}

static inline int tlv320aic3120_mute_output(struct baudio_inst *inst, bool mute)
{
    int err = 0;

    err |= tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
    // Select Page 0
    tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
    // tlv320_write_byte_check(inst, 0x40 0C // mute DACs
    tlv320_write_byte_check(inst, TVL320_DAC_MUTE_CONTROL_REG64, (mute) ? 0x0C : 0x04);
    // f 30 26 xxx1xxx1 // wait for DAC gain flag to be set
    // tlv320_poll_for_reg_val(inst, 0x26, 0x10, 0x10);
    return 0;
}

static inline int tlv320aic3120_set_output_volume(struct baudio_inst *inst, int16_t db_val_x100)
{
    if (db_val_x100 < -6350) {
        db_val_x100 = 6350;
    }
    if (db_val_x100 > 2400) {
        db_val_x100 = 2400;
    }
    db_val_x100 *= 2;
    uint8_t vol = db_val_x100 / 100;
    int ret = tlv320_write_byte_check(inst, TVL320_DAC_VOLUME_CONTROL_REG65, vol);
    ESP_LOGD(TAG, "Set volume reg:%x db:%d", vol, db_val_x100);
    // f 30 26 xxx1xxx1 // wait for DAC gain flag to be set
    tlv320_poll_for_reg_val(inst, 0x26, 0x10, 0x10);
    return ret;
}

static int tlv320aic3120_set_input_mute(struct baudio_inst *inst, uint8_t is_enabled)
{
    if (inst->cur_output != OUTPUT_1_SPK_MONO) {
        return -EAGAIN;
    }
    uint8_t reg_value = 0;
    // Select Page 1
    tlv320_write_byte_check(inst, TVL320_PAGE_1_CONTROL_REG00, TVL320_PAGE_1);

    // mute HPL control
    reg_value = tlv320_read_byte(inst, 0x10) & 0xBF; // reset mute bit
    reg_value |= (is_enabled << 6);                  // set value for mute bit
    tlv320_write_byte(inst, 0x10, reg_value);
    // mute HPR control
    reg_value = tlv320_read_byte(inst, 0x11) & 0xBF; // reset mute bit
    reg_value |= (is_enabled << 6);                  // set value for mute bit
    tlv320_write_byte(inst, 0x11, reg_value);
    return 0;
}

static int tlv320aic3120_set_output_gain(struct baudio_inst *inst, int8_t gain)
{
    if (inst->cur_output != OUTPUT_1_SPK_MONO) {
        return -EAGAIN;
    }
    if ((gain < -6) || (gain > 29)) {
        return -EINVAL;
    }
    uint8_t reg_value = 0;
    // Select Page 1
    tlv320_write_byte_check(inst, TVL320_PAGE_1_CONTROL_REG00, TVL320_PAGE_1);
    // gain HPL control
    reg_value = tlv320_read_byte(inst, 0x10) & 0xC0; // reset gain bits
    reg_value |= (uint8_t)(gain & 0x3F);             // set gain value bits
    tlv320_write_byte(inst, 0x10, reg_value);
    // gain HPR control
    reg_value = tlv320_read_byte(inst, 0x11) & 0xC0; // reset gain bits
    reg_value |= (uint8_t)(gain & 0x3F);             // set gain value bits
    tlv320_write_byte(inst, 0x11, reg_value);
    return 0;
}

static int tlv320aic3120_LDO_PowerCtrl(struct baudio_inst *inst, uint8_t is_enabled)
{
    uint8_t reg_value = 0;
    // Select Page 1
    tlv320_write_byte_check(inst, TVL320_PAGE_1_CONTROL_REG00, TVL320_PAGE_1);
    // read LDO control register
    reg_value = tlv320_read_byte(inst, 0x02) & 0xFE;
    // write LDO power state
    tlv320_write_byte(inst, 0x02, reg_value | is_enabled);
    return 0;
}

static int tlv320aic3120_select_input(struct baudio_inst *inst, enum baudio_inputs input)
{
    uint8_t leftMic_PGA_VolCtrl = 0, rightMicPGA_VolCtrl = 0;
    if (inst->cur_input != input) {
        // change current outputs
        inst->cur_input = input;
        // Select Page 1
        tlv320_write_byte_check(inst, TVL320_PAGE_1_CONTROL_REG00, TVL320_PAGE_1);
        // read left and right MicPGA volume control registers
        leftMic_PGA_VolCtrl = tlv320_read_byte(inst, 0x3B);
        rightMicPGA_VolCtrl = tlv320_read_byte(inst, 0x3C);
        // enable inputs mute
        tlv320_write_byte(inst, 0x3B, leftMic_PGA_VolCtrl | 0x80);
        tlv320_write_byte(inst, 0x3C, rightMicPGA_VolCtrl | 0x80);

        // Route IN1L to LEFT_P with 20K input impedance
        tlv320_write_byte(inst, 0x34, 0x80);
        // Route IN1R to RIGHT_P with input impedance of 20K
        tlv320_write_byte(inst, 0x37, 0x80);
        // disable inputs mute
        tlv320_write_byte(inst, 0x3B, leftMic_PGA_VolCtrl & 0x7F);
        tlv320_write_byte(inst, 0x3C, rightMicPGA_VolCtrl & 0x7F);
    }
    return 0;
}

static int tlv320aic3120_select_output(struct baudio_inst *inst, enum baudio_outputs outputs)
{
    if (inst->cur_output != outputs) {
        // enable outputs mute
        tlv320aic3120_set_input_mute(inst, 1);
        // change current outputs
        inst->cur_output = outputs;
        // Select Page 1
        tlv320_write_byte_check(inst, TVL320_PAGE_1_CONTROL_REG00, TVL320_PAGE_1);
        // Route Left DAC to HPL
        tlv320_write_byte(inst, 0x0C, 0x08);
        // Route Right DAC to HPR
        tlv320_write_byte(inst, 0x0D, 0x08);
        // Unroute Left DAC from LOL
        tlv320_write_byte(inst, 0x0E, 0x00);
        // Unroute Right DAC from LOR
        tlv320_write_byte(inst, 0x0F, 0x00);
        // Power up HPL and HPR drivers
        tlv320_write_byte(inst, 0x09, 0x30);
        // disable outputs mute
        tlv320aic3120_set_input_mute(inst, 0);
        // Set out driver gain -1 dB (for preventing distortion at max volume on loudspeakers)
        tlv320aic3120_set_output_gain(inst, (outputs == OUTPUT_1_SPK_MONO) ? (-1) : (0));
    }
    return 0;
}

static int tlv320aic3120_beep(struct baudio_inst *inst, uint16_t freq, uint16_t ms)
{
    uint32_t u32val;
    // tlv320_write_byte_check(inst, 0x00 00 // change to Page 0
    tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
    //
    tlv320aic3120_mute_output(inst, true);
    // tlv320_write_byte_check(inst, 0x0B 02 // power down NDAC divider
    tlv320_write_byte(inst, 0x0B, 0x02);
    // tlv320_write_byte_check(inst, 0x47 80 // enable beep generator with left channel volume = 0dB, volume level could be different as per requirement
    tlv320_write_byte(inst, 0x47, 0x80);
    // tlv320_write_byte_check(inst, 0x0B 82 // power up NDAC divider, in this specific example NDAC = 2, but NDAC could be different value as per overall setup
    tlv320_write_byte(inst, 0x0B, 0x82);
    // tlv320_write_byte_check(inst, 0x40 00 // un-mute DAC to resume playing audio
    tlv320aic3120_mute_output(inst, false);
    //
    // write sample duration
    u32val = 160 * ms;
    u32val = __bswap32(u32val);
    bi2c_write_reg_array(inst->cfg->com.i2c, 0x49, ((uint8_t *)&u32val) + 1, 3);
    // write sample frequency
    u32val = freq;
    u32val <<= 16;
    u32val |= freq;
    u32val = __bswap32(u32val);
    bi2c_write_reg_array(inst->cfg->com.i2c, 0x4C, (uint8_t *)&u32val, 4);

    // enable beep generator and set volume -6 dB
    tlv320_write_byte(inst, 0x47, 0x86);
    return 0;
}

static uint16_t tlv320aic3120_get_output_pending(struct baudio_inst *inst)
{
    // return (uint16_t)(__HAL_DMA_GET_COUNTER(hi2s2.hdmatx) & 0xFFFF);
    return 0;
}

static uint16_t tlv320aic3120_get_input_pending(struct baudio_inst *inst)
{
    // return (uint16_t)(__HAL_DMA_GET_COUNTER(hi2s2.hdmarx) & 0xFFFF);
    return 0;
}

static int tlv320aic3120_start_transfer(struct baudio_inst *inst, uint16_t *tx_data, uint16_t *rx_data, uint16_t size)
{
    // HAL_I2SEx_TransmitReceive_DMA(&hi2s2, tx_data, rx_data, size);
    return 0;
}

static int tlv320aic3120_reset(struct baudio_inst *inst)
{
    int err = 0;
    vTaskDelay(2);
    if (inst->cfg->reset_gpio_num == -1) {
        // Set register page to 0
        err |= tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
        // Initiate SW Reset
        err |= tlv320_write_byte(inst, TVL320_SOFTWARE_RESET_REG01, 0x01);
    } else {
        //	make hardware reset (pin PB10)
        err |= gpio_set_level(inst->cfg->reset_gpio_num, 0);
        vTaskDelay(1);
        err |= gpio_set_level(inst->cfg->reset_gpio_num, 1);
    }
    //	wait analog and PLL startup
    vTaskDelay(2);
    return err;
}

static int tlv320aic3120_set_input_sample_rate(struct baudio_inst *inst, uint32_t sample_rate)
{
    tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
    tlv320_write_byte(inst, 0x12, 0x80 | 0x8); // Enable NADC and set to 8
    tlv320_write_byte(inst, 0x13, 0x80 | 0xc); // Enable MADC and set to 12
    tlv320_write_byte(inst, 0x14, 128);        // set AOSR to 128
    return 0;
}

static int tlv320aic3120_set_output_sample_rate(struct baudio_inst *inst, uint32_t sample_rate)
{
    int err = tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
    // Program and power up NDAC
    err |= tlv320_write_byte_check(inst, TVL320_DAC_NDAC_VAL_REG11, 0x88);
    // Program and power up MDAC
    err |= tlv320_write_byte_check(inst, TVL320_DAC_MDAC_VAL_REG12, 0x82);
    // Program OSR value
    // DOSR = 128, DOSR(9:8) = 0, DOSR(7:0) = 128
    err |= tlv320_write_byte_check(inst, TVL320_DAC_DOSR_VAL_MSB_REG13, 0x00);
    err |= tlv320_write_byte_check(inst, TVL320_DAC_DOSR_VAL_LSB_REG14, 0x80);
    return 0;
}

static int tlv320aic3120_set_enable_pll(struct baudio_inst *inst, uint8_t p, uint8_t r, uint8_t j)
{
    int err = 0;
    // PLL CLK = MCLK * (r * j.d)/p
    uint64_t freq = inst->cfg->freq;
    freq *= r;
    freq *= j;
    freq /= p;
    inst->codec_freq = freq;

    err |= tlv320_write_byte_check(inst, TVL320_PAGE_0_CONTROL_REG00, TVL320_PAGE_0);
    err |= tlv320_write_byte_check(inst, TVL320_CLOCK_GEN_MUXING_REG04, 0x03);                                    // PPL_in = MCLK, CODEC_in = PLL_CLK
    err |= tlv320_write_byte_check(inst, TVL320_PLL_J_VALUE_REG06, j);                                            // write j
    err |= tlv320_write_byte_check(inst, TVL320_PLL_D_VALUE_MSB_REG07, 0x00);                                     // write d
    err |= tlv320_write_byte_check(inst, TVL320_PLL_D_VALUE_LSB_REG08, 0x00);                                     // write d
    err |= tlv320_write_byte_check(inst, TVL320_PLL_P_AND_R_VALUES_REG05, (0x80 | ((p & 0x7) << 4) | (r & 0xF))); // start pll and set p/r values
    return err;
}

// ps_iic_wr {dev_addr reg_addr reg_data iic_baseaddr args}
void tlv320_clock_cfg(struct baudio_inst *inst)
{
    // ----------------------------------------------------------------------------
    // Clock configurations : Copy from the excel
    // ----------------------------------------------------------------------------
    // Selecting page 0
    tlv320_write_byte_check(inst, 0x00, 0x00);
    // Reset
    tlv320_write_byte(inst, 0x01, 0x01);
    // clock_gen_muxing : PLL_CLK_IN - MCLK, CODEC_CLKIN - PLL_CLK
    tlv320_write_byte_check(inst, 0x04, 0x03);
    // PLL P and R Values : PLL_POWER_ON/OFF - ON, P - 1, R - 1
    tlv320_write_byte_check(inst, 0x05, 0x11);
    // Enable PLL
    tlv320_write_byte_check(inst, 0x05, 0x91);
    // PLL J-Value : J - 7, 
    tlv320_write_byte_check(inst, 0x06, 0x07);
    // PLL D_MSB : D - 0
    tlv320_write_byte_check(inst, 0x07, 0x00);
    tlv320_write_byte_check(inst, 0x08, 0x00);
    // DAC NDAC_VAL : NDAC - 7, DAC_CLK - 12.288MHz
    tlv320_write_byte_check(inst, 0x0B, 0x87);
    // DAC MDAC_VAL : DAC MDAC ON/OFF - ON, MDAC - 6
    tlv320_write_byte_check(inst, 0x0C, 0x86);
    // DAC DOSR_VAL MSB : DOSR - 128
    tlv320_write_byte_check(inst, 0x0D, 0x00);
    // DAC DOSR_VAL LSB :
    tlv320_write_byte_check(inst, 0x0E, 0x80);
    // ADC NADC_VAL : ADC NDAC ON/OFF - ON, NADC - 7
    tlv320_write_byte_check(inst, 0x12, 0x87);
    // ADC MADC_VAL : ADC MDAC ON/OFF - ON, MADC - 6
    tlv320_write_byte_check(inst, 0x13, 0x86);
    // ADC AOSR_VAL : AOSR - 128
    tlv320_write_byte_check(inst, 0x14, 0x80);
    // Set GPIO1 = CLKOUT1
    tlv320_write_byte_check(inst, 0x33, 0x12);
    // Set ADC_CLK as CLK input for CDIV
    tlv320_write_byte_check(inst, 0x19, 0x07);
    // Set M factor for CDIVE
    tlv320_write_byte_check(inst, 0x1A, 0x80);
    
    // ----------------------------------------------------------------------------
    // I2S configurations : Copy from the excel
    // ----------------------------------------------------------------------------
    // Codec Interface Ctrl : WCLK_IN/OUT - OUT, BCLK_IN/OUT - OUT, Mode - DSP, Word length - 16
    tlv320_write_byte_check(inst, 0x1B, 0x4C);
    // Data-slot offeset prog. : offset - 1
    tlv320_write_byte_check(inst, 0x1C, 0x01);
    // Codec interface ctrl 2 : BCLK_SRC - ADC_MOD_CLK, DIN_DOUT_LOOPBACK - DISABLE, ADC_DAC_LOOPBACK - DISABLE, BCLK & WCLK active when PD - DISABLE
    tlv320_write_byte_check(inst, 0x1D, 0x03);
    // BCLK N divider value : BCLK_N_DIV_ON/OFF - ON, BCLK_DIV - 4
    tlv320_write_byte_check(inst, 0x1E, 0x83);
    //  tlv320_write_byte_check(inst, 0x1E 0x04 );
    // codec sec interface ctrl 1 : Sec BCLK - frm GPIO1, Sec WCLK - from GPIO1, Sec Din - from GPIO1
    tlv320_write_byte_check(inst, 0x1F, 0x00);
    // codec sec interface ctrl 2 : ADC_WCLK - from GPIO1, prim BCLK - fed to ck_gen and codec int., prim WCLK - fe o codec int., ADC_WCLK - same as DAC_WCLK, prim Din - fed to codec int.
    tlv320_write_byte_check(inst, 0x20, 0x00);
    // codec sec interface ctrl 3 : prim BCLK out - in. BCLK, sec BCLK out - prim BCLK, prim WCLK out - ADC_f, swc WCLK ot  prim WCLK, prim DOUT - dout, sec DOUT - prim DIN
    tlv320_write_byte_check(inst, 0x21, 0x10);
    // ----------------------------------------------------------------------------
    // Processing block configurations : Copy from the excel
    // ----------------------------------------------------------------------------
    // DAC Processing Block / miniDSP Selection : PRB_P4
    tlv320_write_byte_check(inst, 0x3C, 0x04);
    // ADC Processing Block / miniDSP Selection : PRB_R4
    tlv320_write_byte_check(inst, 0x3D, 0x04);
}

void tlv320_speaker_config(struct baudio_inst *inst)
{
    // selecting page 0 
    tlv320_write_byte_check(inst, 0x00, 0x00);

    tlv320_write_byte_check(inst, 0x74, 0x00);
    tlv320_write_byte_check(inst, 0x44, 0x00);
    tlv320_write_byte_check(inst, 0x41, 0X00);
    tlv320_write_byte_check(inst, 0x00, 0x01);
    tlv320_write_byte_check(inst, 0x23, 0X40);
    tlv320_write_byte_check(inst, 0x2a, 0X04);
    tlv320_write_byte_check(inst, 0x20, 0xc6);
    tlv320_write_byte_check(inst, 0x26, 0X80);
    tlv320_write_byte_check(inst, 0x00, 0x08);
    tlv320_write_byte_check(inst, 0x01, 0x04);
    tlv320_write_byte_check(inst, 0x00, 0x00);
    tlv320_write_byte_check(inst, 0x3F, 0xd6);
    tlv320_write_byte_check(inst, 0x40, 0X00);
}

typedef struct {
    uint8_t reg;
    uint8_t val;
} tlv_cmd_t;

// Clock-gen muxing. PLL_CLKIN=MCLK, CODEC_CLKIN=PLL_CLK 12.288MHZ
// P=1, R=1, J=6, D=10000,
// NDAC=NADC=7, MDAC=MADC=6,
// DOSR=AOSR=128 => 16KHz fs

static const tlv_cmd_t init_cmd[] = {
    {0x00, 0x00}, // Page 0
    {0x01, 0x01}, // sw reset
    // PLL config
    {0x04, 0x03}, // Clock-gen Mux. PLL_CLKIN=MCLK, CODEC_CLKIN=PLL_CLK
    {0x05, 0x91}, // PLL P+R. PLL power up, P=1, R=1 (P=divider,R=multiplier)
    {0x06, 0x06}, // PLL J = 6
    {0x07, 0x27}, // PLL D fractional. D-value MSB = 0x27
    {0x08, 0x10}, // PLL D fractional. D-value LSB = 0x10 -> D=10000
    // mode master
    {0x1B, 0x0C}, // mode is i2s master, wordlength is 16, BCLK/WCLK out
    // divider configs
    {0x0B, 0x87}, // NDAC is powered up and set to 7
    {0x0C, 0x86}, // MDAC is powered up and set to 6
    {0x0D, 0x00}, // DOSR = 128 0x080, DOSR(9:8) = 0x00
    {0x0E, 0x80}, // DOSR = 128 0x080, DOSR(7:0) = 0x80
    {0x12, 0x87}, // NADC is powered up and set to 7
    {0x13, 0x86}, // MADC is powered up and set to 6
    {0x14, 0x80}, // AOSR = 128 0x080, AOSR(7:0) = 0
    //
    {0x74, 0x00}, // DAC => volume control thru pin disable
    {0x44, 0x00}, // DAC => drc disable, th and hy
    {0x41, 0x00}, // DAC => 0 db gain left
    //
    {0x00, 0x00}, // Page 0
    {0x1d, 0x1d}, // b4 = ADC-to-DAC loopback enabled
    {0x3C, 0x04}, // Use PRB_P4 as there's no PRB_P11
    {0x3D, 0x04}, // Use PRB_R4 default
                  //
    {0x00, 0x01}, // Page 1
    {0x21, 0x4E}, // De-pop, Power on = 800 ms, Step time = 4 ms
    {0x1F, 0x84}, // HPOUT powered up

    {0x23, 0x40}, // LDAC routed to HPL
    {0x28, 0x0E}, // HPOUT unmute and gain 1dB
    {0x24, 0x00}, // No attenuation on HP
    {0x2E, 0x0A}, // MIC BIAS = 2.5v
    {0x30, 0x40}, // MICPGA P = MIC1LP 10K
    {0x31, 0x10}, // MICPGA N = MIC1LM 10K
    //
    {0x00, 0x08}, // Page 8
    {0x01, 0x04}, // Enable DAC Adaptive filter
    //
    {0x00, 0x01}, // Page 1
    {0x26, 0x4c}, // Class-D Analog volume attenuation
    {0x2A, 0x1C}, // Unmute Class-D left with 24dB gain
    {0x20, 0x86}, // Power-up Class-D drivers

    {0x00, 0x00}, // Page 0
    {0x3F, 0x96}, // POWERUP DAC with Left data (soft step disable)
    {0x40, 0x04}, // Unmute DAC
    {0x51, 0x80}, // Powerup ADC channel
    {0x52, 0x00}, // Unmute ADC channel
};

#if 0
#if 1
static const tlv_cmd_t loop_back_cmd[] = {
    {0x00, 0x00}, // Page 0
    {0x1d, 0x1d}, // b4 = ADC-to-DAC loopback enabled
    {0x00, 0x01},
    {0x2e, 0x0a}, // MICBIAS = 2.5V
    {0x26, 0x4c},
};
#else
static const tlv_cmd_t beep_cmd[] = {
    {0x00, 0x00}, // Page 0
    {0x40, 0x0C}, // mute DACs
    {0x0B, 0x07}, // power down NDAC divider
    {0x47, 0x80}, // enable beep generator with left channel volume = 0dB
    {0x49, 0x00}, // BeepLen HIGH
    {0x4A, 0x7D}, // BeepLen MID
    {0x4B, 0xA0}, // BeepLen LOW
    {0x4C, 0x59}, // BeepSin MSB
    {0x4D, 0x98}, // BeepSin LSB
    {0x4E, 0x59}, // BeepCos MSB
    {0x4F, 0x98}, // BeepCos LSB
    {0x0B, 0x87}, // NDAC is powered up and set to 7
    {0x40, 0x04}, // Unmute DAC
};
#endif
#endif

static void tlv320_send_cmds(struct baudio_inst *inst, const tlv_cmd_t *cmd, uint8_t cmd_len)
{
    while (cmd_len) {
        tlv320_write_byte_check(inst, cmd->reg, cmd->val);
        cmd_len--;
        cmd++;
    }
}

void tlv320_beep_gen(struct baudio_inst *inst)
{
#if 0
#if 1
    tlv320_send_cmds(inst, loop_back_cmd, ARRAY_LEN(loop_back_cmd));
#else
    tlv320_send_cmds(inst, beep_cmd, ARRAY_LEN(beep_cmd));
#endif
#endif
}

static int tlv320aic3120_init(struct baudio_inst *inst)
{
    tlv320_send_cmds(inst, init_cmd, ARRAY_LEN(init_cmd));
    return 0;
}

static int tlv320aic3120_deinit(struct baudio_inst *inst)
{
    tlv320aic3120_set_input_mute(inst, 1); // mute channels
    tlv320aic3120_LDO_PowerCtrl(inst, 0);  // LDO power down
    return 0;
}

BAUDIO_API_DEFINE(tlv320aic3120);
