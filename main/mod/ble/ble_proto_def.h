///
/// @file ble_sensor_proto.h
/// @author your name (you@domain.com)
/// @brief
/// @version 0.1
/// @date 2022-07-03
///
/// @copyright Copyright (c) 2022
///
///
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pmods.h>
#include <utils/butils.h>

enum cs_ble_cmd_ids {
    eCS_BLE_CMD_INIT_SENSOR = 1,
    eCS_BLE_CMD_DAQ_SETTINGS,
    eCS_BLE_CMD_DAQ_STATE,
    eCS_BLE_CMD_FRAG_ACK,
} PACKED_FMT;

enum cs_ble_sensor_state {
    eCS_SENSOR_STATE_IDLE = 0,
    eCS_SENSOR_STATE_RUNNING,
    eCS_SENSOR_STATE_PAUSED,
} PACKED_FMT;

struct cs_ble_proto_sensor_init_cmd {
    uint8_t sensor_type;        ///<
    uint8_t sensor_addr;        ///<
    uint8_t sensor_settings[0]; ///<
} PACKED_FMT;

struct cs_ble_proto_sensor_state {
    uint8_t sensor_type;        ///<
    uint8_t state;              ///<
    bool is_remote;             ///<
    uint8_t err_code;           ///<
    uint8_t sensor_settings[0]; ///<
} PACKED_FMT;

struct cs_ble_proto_daq_setting {
    uint32_t start_unix_timestamp; ///<
    uint32_t daq_interval_ms;      ///<
    uint32_t daq_duration_ms;      ///<
} PACKED_FMT;

struct cs_ble_proto_data_samples {
    uint8_t sensor_type;    ///<
    uint8_t err_code;       ///<
    uint32_t sample_length; ///< size depends on sensor data types
    uint8_t payload[0];     ///<
} PACKED_FMT;

struct cs_ble_cmd_pkt {
    enum cs_ble_cmd_ids id;
    union {
        struct cs_ble_proto_sensor_init_cmd init_cmd;
        struct cs_ble_proto_sensor_state state_cmd;
        struct cs_ble_proto_daq_setting daq_cfg;
    };
} PACKED_FMT;

struct cs_ble_data_pkt {
    union {
    } arr[0];
} PACKED_FMT;

struct cs_ble_proto_frag_msg {
    uint8_t frag_idx; ///<
    uint8_t frag_cnt; ///<
    union {
        uint8_t payload[180]; ///<
        struct cs_ble_cmd_pkt cmd[0];
        struct cs_ble_data_pkt data[0];
    };
} PACKED_FMT;