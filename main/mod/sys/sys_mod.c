///
/// @file sys_mod.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-03-22
///
/// @copyright Copyright (c) 2024
///
///
#include <mod/sys/sys_mod.h>
#include <utils/sll_listeners.h>
#include <utils/util_macro.h>
#include <lib/bioexp/bioexp.h>
#include "esp_log.h"

static const char *TAG = "mSYS";

void on_usr_btn_n_chg_state_changed_callback(enum bioexp_ios io_mask, uint32_t io_states)
{
    const uint32_t btn_mask = BIT(BIOEXP_BTN_0) | BIT(BIOEXP_BTN_1);
    if (btn_mask & io_mask) {
		uint8_t state = bioexp_get(BIOEXP_BTN_1);
		state <<= 1;
		state |= bioexp_get(BIOEXP_BTN_0);
        ESP_LOGI(TAG, "BTN 0/1 State changed %d", state);
    }
    //
    const uint32_t chg_state_mask = BIT(BIOEXP_STAT_0) | BIT(BIOEXP_STAT_1) | BIT(BIOEXP_PG);
    if (chg_state_mask & io_mask) {
		uint8_t state = bioexp_get(BIOEXP_PG);
		state <<= 1;
		state |= bioexp_get(BIOEXP_STAT_1);
		state <<= 1;
		state |= bioexp_get(BIOEXP_STAT_0);
        ESP_LOGI(TAG, "CHG State changed %d", state);
    }

    const uint32_t pmod_detect_mask = BIT(BIOEXP_DETECT_1);
    if (pmod_detect_mask & io_mask) {
		uint8_t state = bioexp_get(BIOEXP_DETECT_1);
        ESP_LOGI(TAG, "PMOD Detect Changed %d", state);
    }
}

SLL_LISTENER_DEFINE_NODE(bioexp, on_usr_btn_n_chg_state_observer, on_usr_btn_n_chg_state_changed_callback);

void sys_mod_init(void)
{
    SLL_LISTENER_ADD_NODE(bioexp, on_usr_btn_n_chg_state_observer);
}