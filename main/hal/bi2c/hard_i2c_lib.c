///
/// @file hard_i2c_lib.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2024-02-14
///
/// @copyright Copyright (c) 2024
///
///
#include <errno.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "hard_i2c_lib.h"

static const char *TAG = "bi2c";

#define DEFAULT_RETRY_COUNT 100

int hard_i2c_lib_init(const struct bi2c_inst *inst)
{
    i2c_param_config(I2C_NUM_0, (const i2c_config_t *)inst->cfg);
    return i2c_driver_install(I2C_NUM_0, ((const i2c_config_t *)inst->cfg)->mode, 0, 0, 0);
}

int hard_i2c_lib_write_bytes(const struct bi2c_inst *inst, const struct bi2c_slave *slave,
                             const uint8_t *cmd, uint16_t cmd_len,
                             const uint8_t *data, uint16_t data_len)
{
    int err;
    uint8_t buffer[(5 + (6 * 7)) * 4] = {0};
    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    assert(handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }
    err = i2c_master_write_byte(handle, slave->addr << 1 | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        goto end;
    }
    if (cmd && cmd_len) {
        err = i2c_master_write(handle, cmd, cmd_len, true);
        if (err != ESP_OK) {
            goto end;
        }
    }
    switch (slave->write_mode) {
    case BI2C_STOP_START:
        err = i2c_master_stop(handle);
        if (err != ESP_OK) {
            goto end;
        }
    case BI2C_RESTART:
        err = i2c_master_start(handle);
        if (err != ESP_OK) {
            goto end;
        }
        err = i2c_master_write_byte(handle, slave->addr << 1 | I2C_MASTER_WRITE, true);
        if (err != ESP_OK) {
            goto end;
        }
        break;
    default:
        break;
    }
    if (cmd && cmd_len) {
        err = i2c_master_write(handle, data, data_len, true);
        if (err != ESP_OK) {
            goto end;
        }
    }
    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(I2C_NUM_0, handle, pdMS_TO_TICKS(DEFAULT_RETRY_COUNT));
end:
    i2c_cmd_link_delete_static(handle);
    ESP_LOGE(TAG, "I2C:%d, err: %d", slave->id, err);
    return err;
}

int hard_i2c_lib_read_bytes(const struct bi2c_inst *inst,
                            const struct bi2c_slave *slave,
                            const uint8_t *cmd, uint16_t cmd_len,
                            uint8_t *buf, uint16_t buf_len)
{
    int err;
    uint8_t buffer[(5 + (7 * 7)) * 4] = {0};
    i2c_cmd_handle_t handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    assert(handle != NULL);

    err = i2c_master_start(handle);
    if (err != ESP_OK) {
        goto end;
    }
    err = i2c_master_write_byte(handle, slave->addr << 1 | I2C_MASTER_WRITE, true);
    if (err != ESP_OK) {
        goto end;
    }
    err = i2c_master_write(handle, cmd, cmd_len, true);
    if (err != ESP_OK) {
        goto end;
    }
    switch (slave->read_mode) {
    case BI2C_STOP_START:
        err = i2c_master_stop(handle);
        if (err != ESP_OK) {
            goto end;
        }
    case BI2C_RESTART:
        err = i2c_master_start(handle);
        if (err != ESP_OK) {
            goto end;
        }
        err = i2c_master_write_byte(handle, slave->addr << 1 | I2C_MASTER_READ, true);
        if (err != ESP_OK) {
            goto end;
        }
        break;
    default:
        break;
    }
    err = i2c_master_read(handle, buf, buf_len, I2C_MASTER_LAST_NACK);
    if (err != ESP_OK) {
        goto end;
    }
    i2c_master_stop(handle);
    err = i2c_master_cmd_begin(inst->id, handle, pdMS_TO_TICKS(DEFAULT_RETRY_COUNT));
end:
    i2c_cmd_link_delete_static(handle);
    ESP_LOGE(TAG, "I2C:%d, err: %d", slave->id, err);
    return err;
}
