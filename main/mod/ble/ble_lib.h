///
/// @file ble_lib.h
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-07-16
///
/// @copyright Copyright (c) 2022
///
///
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <ble/ble_proto_def.h>

struct ble_callback_config {

    ///
    /// @brief callback when connection state change
    ///
    /// @param connected true/false
    ///
    void (*on_ble_state_change)(bool connected);

    ///
    /// @brief callback to update application with updated MTU size it can use
    ///
    /// @param mtu updated mtu size
    ///
    void (*on_ble_mtu_change)(uint16_t mtu);

    /// 
    /// @brief 
    /// 
    /// @param data  
    /// @param data_len
    /// 
    void (*on_ble_daq_cmd_received)(uint8_t *data, int data_len);
};

/// 
/// @brief to be defined externally thats why constant 
/// 
/// 
extern const struct ble_callback_config ble_callbacks;

extern void ble_lib_init();

extern void ble_lib_send_pending_frame();

extern int ble_lib_send_data(uint8_t *data, uint16_t data_len);