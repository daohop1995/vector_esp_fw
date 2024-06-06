///
/// @file ble_lib.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2022-07-16
///
/// @copyright Copyright (c) 2022
///
///
#include "ble_lib.h"
#include <utils/butils.h>

static struct
{
    uint8_t *data_ptr;
    uint16_t data_len;
    uint16_t mtu_size;
    bool notifications_enabled;
} _stm = {
    .data_ptr = NULL,
    .data_len = 0,
    .mtu_size = CYBLE_GATT_DEFAULT_MTU,
    .notifications_enabled = true,
};
static struct cs_ble_proto_frag_msg _daq_frag;
static CYBLE_GATTS_HANDLE_VALUE_NTF_T _daq_handle = {
    .attrHandle = CYBLE_CAPSENSE_CAPSENSEDAQEXCHG_CHAR_HANDLE,
    .value.val = (uint8_t *)&_daq_frag,
};

void ble_lib_send_pending_frame()
{
    CYBLE_GATT_ERR_CODE_T ret1;
    CYBLE_API_RESULT_T ret2;
    while ((CyBle_GetState() == CYBLE_STATE_CONNECTED) 
            && (_stm.data_ptr) 
            && (_daq_frag.frag_idx < _daq_frag.frag_cnt)) {
        //
        uint16_t daq_send_idx = _stm.mtu_size * _daq_frag.frag_idx;
        uint16_t btw = min(_stm.data_len - daq_send_idx, _stm.mtu_size);
        //
        _daq_handle.value.len = 2 + btw;
        memcpy(_daq_frag.payload, &_stm.data_ptr[daq_send_idx], btw);
        //
        ret1 = CyBle_GattsWriteAttributeValue(&_daq_handle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
        if(ret1== CYBLE_GATT_ERR_NONE){
            ret2 = CyBle_GattsNotification(cyBle_connHandle, &_daq_handle);
            // ret2 = CyBle_GattsIndication(cyBle_connHandle, &_daq_handle);
            if(ret2 == CYBLE_ERROR_OK){
                _daq_frag.frag_idx++;
            }  else {
                break;
            }
        }
    }
}

static void ble_stack_callback(uint32 event, void *eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

    switch (event) {
    /* if there is a disconnect or the stack just turned on from a reset then start the advertising and turn on the LED blinking */
    case CYBLE_EVT_STACK_ON:
    {
        CYBLE_GAP_BD_ADDR_T bdAddr;
        CyBle_GetDeviceAddress(&bdAddr);
        const char hexmap[] = "0123456789ABCDEF";
        char name[] = "Vector_XXXX";
        name[10] = hexmap[((bdAddr.bdAddr[0] >> 0) &0x0F)];
        name[9] = hexmap[((bdAddr.bdAddr[0] >> 4) &0x0F)];
        name[8] = hexmap[((bdAddr.bdAddr[1] >> 0) &0x0F)];
        name[7] = hexmap[((bdAddr.bdAddr[1] >> 4) &0x0F)];
        CyBle_GapSetLocalName(name);
    }
    case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        //CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_SLOW);
        CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        ble_callbacks.on_ble_state_change(false);
        _stm.mtu_size = CYBLE_GATT_DEFAULT_MTU;
        _stm.notifications_enabled = false;
        _daq_frag.frag_idx = 0;
        _daq_frag.frag_cnt = 0;
        break;

    /* when a connection is made, update the LED and Capsense states in the GATT database and stop blinking the LED */
    case CYBLE_EVT_GATT_CONNECT_IND:
        ble_callbacks.on_ble_state_change(true);
        break;

    case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        _stm.mtu_size = (((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu < CYBLE_GATT_MTU) ? ((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu : CYBLE_GATT_MTU;
        _stm.mtu_size -= 3; // adjust ble header
        _stm.mtu_size -= 2; // adjust pkt header
        if(_stm.mtu_size > sizeof(_daq_frag.payload)){
           _stm.mtu_size = sizeof(_daq_frag.payload);
        }
        break;

    /* handle a write request */
    case CYBLE_EVT_GATTS_WRITE_REQ:
        wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *)eventParam;
        if (wrReqParam->handleValPair.attrHandle == CYBLE_CAPSENSE_CAPSENSEDAQEXCHG_CHAR_HANDLE) {
            CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);
            ble_callbacks.on_ble_daq_cmd_received(wrReqParam->handleValPair.value.val, wrReqParam->handleValPair.value.len);
            CyBle_GattsWriteRsp(cyBle_connHandle);
        }
        if (wrReqParam->handleValPair.attrHandle == CYBLE_CAPSENSE_CAPSENSEDAQEXCHG_AINCCCD_DESC_HANDLE) {
            CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);
            _stm.notifications_enabled = wrReqParam->handleValPair.value.val[0] & 0x03;
            CyBle_GattsWriteRsp(cyBle_connHandle);
        }
        break;

    case CYBLE_EVT_GATTS_HANDLE_VALUE_CNF:
        // ble_lib_send_pending_frame();
        break;

    default:
        break;
    }
}

void ble_lib_init()
{
    CyBle_Start(ble_stack_callback);
}

int ble_lib_send_data(uint8_t *data, uint16_t data_len)
{
    if (_daq_frag.frag_idx < _daq_frag.frag_cnt) {
        return -EBUSY;
    }
    if (CyBle_GetState() != CYBLE_STATE_CONNECTED) {
        return -ENOTCONN;
    }
    if (!_stm.notifications_enabled) {
        return -EAGAIN;
    }
    _daq_frag.frag_idx = 0;
    _daq_frag.frag_cnt = (data_len + _stm.mtu_size - 1) / _stm.mtu_size;
    _stm.data_ptr = data;
    _stm.data_len = data_len;
    ble_lib_send_pending_frame();
    return 0;
}