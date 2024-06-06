///
/// @file rgb_led.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-06-17
///
/// @copyright Copyright (c) 2023
///
///
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "hal/brgb/led_strip.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_TX_GPIO 15
#define STRIP_LED_NUMBER 4

static const char *TAG = "bRGB";

static led_strip_t *strip;

int brgb_init(void)
{
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;
    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(STRIP_LED_NUMBER, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip)
    {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    // Show simple rainbow chasing pattern
    ESP_LOGI(TAG, "LED Rainbow Chase Start");
    return 0;
}

int brgb_set(uint8_t led_num, uint8_t r, uint8_t g, uint8_t b)
{
    ESP_ERROR_CHECK(strip->set_pixel(strip, led_num, r, g, b));
    ESP_ERROR_CHECK(strip->refresh(strip, 100));
    return 0;
}