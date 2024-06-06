///
/// @file main.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-12-09
///
/// @copyright Copyright (c) 2023
///
///
#include "nvs_flash.h"
#include <main.h>
#include <mod/daq/daq_mod.h>
#include <mod/ble/ble_mod.h>
#include <mod/sys/sys_mod.h>
#include <hal/bi2c/bi2c.h>
#include <hal/brgb/brgb.h>
#include <hal/bfs/bfs.h>
#include <utils/sll_listeners.h>
#include <lib/bioexp/bioexp.h>

void app_main(void)
{
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());
        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    
    audio_config();
    //
    bi2c_init(BI2C_0);
    bi2c_init(BI2C_1);
    brgb_init();
    bioexp_init();
    // bfs_mount(BFS_SDMMC_0);
    //
    sys_mod_init();
    ble_mod_init();
    daq_mod_init();
}
