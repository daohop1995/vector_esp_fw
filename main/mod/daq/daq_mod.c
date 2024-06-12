///
/// @file daq_mod.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-12-09
///
/// @copyright Copyright (c) 2023
///
///
#include "daq_mod.h"
#include "hal/bi2c/soft_i2c_lib.h"
#include "hal/brgb/brgb.h"
#include "lib/pmods/pmods.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "lib/baudio/baudio_codec.h"
#include "lib/bhaptic/bhaptic.h"

const char *TAG = "DAQ";

#define DAQ_TASK_STACK_SIZE 4096

int pmod_on_sample_ready(const void *sample, size_t sample_len, bool free_mem)
{
    ESP_LOGI(TAG, "Pmod Ready %d", sample_len);
    return 0;
}

const struct bi2c_slave baudio_i2c_slave = {
    .id = BI2C_1,
    .addr = 0x18,
    .read_mode = BI2C_RESTART,
    .write_mode = BI2C_NO_RESTART,
};
const struct baudio_cfg baudio_cfg = {
    .com = {
        .i2c = &baudio_i2c_slave,
    },
    .reset_gpio_num = -1,
    .freq = 12288000ul};
struct baudio_inst baudio_tlv320 = {
    .cfg = &baudio_cfg,
    .api = &BAUDIO_API_DECLARE(tlv320aic3120),
    .cur_output = OUTPUT_NONE,
    .cur_input = INPUT_MIC_1,
};
struct baudio_inst *baudio = &baudio_tlv320;
void audio_config()
{
    tlv320_clock_cfg(baudio);
    tlv320_speaker_config(baudio);
}
/* Task to be created. */
void daq_task_function(void *pvParameters)
{
    int ret = 0;
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );*/
//
#if 1
    gpio_iomux_in(GPIO_NUM_17, FUNC_U0RXD_U0RXD);
    gpio_iomux_out(GPIO_NUM_17, FUNC_U0RXD_GPIO17, false);
    gpio_set_direction(GPIO_NUM_17, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(GPIO_NUM_17, 0);
#endif
    for (;;) {
#if 0
        const lp_core_i2c_cfg_t i2c_cfg = {
            .i2c_pin_cfg.sda_io_num = GPIO_NUM_6,
            .i2c_pin_cfg.scl_io_num = GPIO_NUM_7,
            .i2c_pin_cfg.sda_pullup_en = 1,
            .i2c_pin_cfg.scl_pullup_en = 1,
            .i2c_timing_cfg.clk_speed_hz = 100000,
            .i2c_src_clk = LP_I2C_SCLK_DEFAULT,
        };
        ret = lp_core_i2c_master_init(LP_I2C_NUM_0, &i2c_cfg);
        ulp_lp_core_cfg_t cfg = {
            .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
        };
        // ret = ulp_lp_core_run(&cfg);
        printf("lpi2c init: %d\n", ret);
        printf("lpi2c scan: \n");
        uint8_t reg = 0;
        uint32_t val = 0;
        lp_core_i2c_master_write_read_device(LP_I2C_NUM_0, 0x20, &reg, sizeof(reg), (uint8_t *)&val, sizeof(val), pdMS_TO_TICKS(1000));
        printf("GPIO-EXP: returns %d, %08X\n", ret, (unsigned int)val);
#endif
#if 0
        // gpio_set_level(GPIO_NUM_17, !gpio_get_level(GPIO_NUM_17));
        printf("i2c scan: \n");
        // uint8_t reg = 0x0f;
        // uint8_t val = 0;
        // struct bi2c_slave slave = {.id = BI2C_0, .addr = 0x19};
        // uint8_t reg = 0x00;
        // uint8_t val = 0;
        // struct bi2c_slave slave = {
        //     .id = BI2C_1,
        //     .addr = 0x20,
        //     .read_mode = BI2C_RESTART,
        //     .write_mode = BI2C_NO_RESTART,
        // };
        uint8_t reg = 0x01;
        uint8_t val = 0;
        struct bi2c_slave slave = {
            .id = BI2C_1,
            .addr = 0x59,
            .read_mode = BI2C_RESTART,
            .write_mode = BI2C_NO_RESTART,
        };
        ret = bi2c_read_bytes(&slave, &reg, sizeof(reg), (uint8_t *)&val, sizeof(val));
        printf("GPIO-EXP: returns %d, %08X\n", ret, (unsigned int)val);
        vTaskDelay(pdMS_TO_TICKS(1000));
#endif
#if 0
        const struct pmod_inst *pmod = pmod_instances[ePMOD_TYPE_ID_ADXL346];
        ret = pmod->api.setup(pmod);
        ESP_LOGI(TAG, "Pmod init %d", ret);
        ret = pmod->api.poll(pmod, pmod_on_sample_ready);
        ESP_LOGI(TAG, "Pmod poll %d", ret);
#endif
#if 0
        static uint8_t color_index = 0;
        static const uint8_t intensity_map[] = {0, 1, 3, 7, 15, 31, 63, 127, 255, 127, 63, 31, 15, 7, 3, 1};
        for (int i = 0; i < 4; i++) {
            brgb_set(i,
                     intensity_map[(color_index + 0 + i) & 0x0F],
                     intensity_map[(color_index + 5 + i) & 0x0F],
                     intensity_map[(color_index + 10 + i) & 0x0F]);
        }
        color_index++;
#endif
#if 0
        uint8_t data = 0;
        int ret = read1bFromRegister(&soft_i2c, 0x19, 0x0F, &data, true);
        printf("GPIO-EXP: returns %d, %08X\n", ret, data);
        vTaskDelay(pdMS_TO_TICKS(1000));
#endif
#if 0
        baudio->api->init(baudio);
        baudio->api->beep(baudio, 1000, 500);
#endif
#if 0
        bhaptic_play_wave(bhaptic_wave_1, ARRAY_LEN(bhaptic_wave_1));
        vTaskDelay(pdMS_TO_TICKS(5000));
        bhaptic_play_wave(bhaptic_wave_2, ARRAY_LEN(bhaptic_wave_2));
        vTaskDelay(pdMS_TO_TICKS(5000));
        bhaptic_play_wave(bhaptic_wave_3, ARRAY_LEN(bhaptic_wave_3));
        vTaskDelay(pdMS_TO_TICKS(5000));
        bhaptic_play_wave(bhaptic_wave_4, ARRAY_LEN(bhaptic_wave_4));
#endif
        ESP_LOGI(TAG, "Sleeping");
        /* Task code goes here. */
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

int daq_mod_init()
{
    BaseType_t xReturned;
    TaskHandle_t xDaqHandle = NULL;

    /* Create the task, storing the handle. */
    xReturned = xTaskCreate(
        daq_task_function,   /* Function that implements the task. */
        "DAQ",               /* Text name for the task. */
        DAQ_TASK_STACK_SIZE, /* Stack size in words, not bytes. */
        (void *)NULL,        /* Parameter passed into the task. */
        tskIDLE_PRIORITY,    /* Priority at which the task is created. */
        &xDaqHandle);        /* Used to pass out the created task's handle. */

    if (xReturned != pdPASS) {
        /* The task was created.  Use the task's handle to delete the task. */
        vTaskDelete(xDaqHandle);
    }

    return xReturned;
}
