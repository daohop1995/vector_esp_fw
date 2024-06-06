#include <project.h>
#include "hal/bi2c/bi2c.h"
#include <ble/ble_lib.h>
#include <utils/butils.h>
#include <mem/mem_lib.h>
#include "pmods.h"
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static const struct pmod_api *sensor_api = NULL;
static struct cs_ble_proto_daq_setting _daq_cfg;
static struct cs_ble_proto_sensor_state _daq_state;
static bool push_sample = false;
static uint32_t timer_ref_ms = 0;

void handle_ble_state_change(bool connected);
void handle_ble_mtu_change(uint16_t mtu);
void handle_ble_daq_cmd_received(uint8_t *data, int data_len);

const struct ble_callback_config ble_callbacks = {
    .on_ble_state_change = handle_ble_state_change,
    .on_ble_mtu_change = handle_ble_mtu_change,
    .on_ble_daq_cmd_received = handle_ble_daq_cmd_received,
};

void handle_ble_state_change(bool connected)
{
    if (connected) {
        pwm_Stop();
    } else {
        pwm_Start();
    }
}

void handle_ble_mtu_change(uint16_t mtu)
{
    (void)mtu;
    // nothing to do
}

CY_ISR(UnixTimerTCIRQHandler)
{
    // here we handle wake of board on predefined time
    push_sample = true;
    timer_ref_ms += _daq_cfg.daq_interval_ms;
}

void handle_ble_daq_cmd_received(uint8_t *data, int data_len)
{
    // struct cs_ble_cmd_pkt *cmd = ((struct cs_ble_proto_frag_msg *)data)->cmd;
    // data_len -= STRUCT_MEMBER_OFFSET(struct cs_ble_proto_frag_msg, cmd); // frag_idx & cnt
    // we are assuming that command pkt will always be less than mtu, thus fragmentation is not required
    struct cs_ble_cmd_pkt *cmd = (struct cs_ble_cmd_pkt *)data;
    switch (cmd->id) {
    case eCS_BLE_CMD_INIT_SENSOR: {
        // init or change sensor here
        if ((data_len - STRUCT_MEMBER_OFFSET(struct cs_ble_cmd_pkt, init_cmd)) >= sizeof(struct cs_ble_proto_sensor_init_cmd)) {
            if (cmd->init_cmd.sensor_type < ePMOD_TYPE_ID_MAX) {
                const struct pmod_api *_new_sensor_api = &k_sensor_spi_table[cmd->init_cmd.sensor_type];
                if (sensor_api && sensor_api->exit) {
                    sensor_api->exit();
                }
                mem_lib_reset(eMEM_IRAM);
                bi2c_setup();
                _new_sensor_api->setup(cmd->init_cmd.sensor_addr, cmd->init_cmd.sensor_settings, data_len - STRUCT_MEMBER_OFFSET(struct cs_ble_cmd_pkt, init_cmd.sensor_settings));
                sensor_api = _new_sensor_api;
                _daq_state.sensor_type = cmd->init_cmd.sensor_type;
            }
        }
    } break;
    case eCS_BLE_CMD_DAQ_STATE: {
        if ((sensor_api) && (cmd->state_cmd.sensor_type == _daq_state.sensor_type)) {
            memcpy(&_daq_state, &cmd->state_cmd, sizeof(_daq_state));
            if (_daq_state.state == eCS_SENSOR_STATE_RUNNING) {
                timer_ref_ms = 0;
                UnixTimer_Start();
            } else {
                UnixTimer_Stop();
            }
        } else {
            _daq_state.sensor_type = ePMOD_TYPE_ID_NULL;
            _daq_state.state = eCS_SENSOR_STATE_IDLE;
        }
    } break;
    case eCS_BLE_CMD_DAQ_SETTINGS: {
        // apply daq setting here
        if ((data_len - STRUCT_MEMBER_OFFSET(struct cs_ble_cmd_pkt, daq_cfg)) == sizeof(struct cs_ble_proto_daq_setting)) {
            memcpy(&_daq_cfg, &cmd->daq_cfg, sizeof(_daq_cfg));
            // set timer update as per settings
            UnixTimer_Stop();
            UnixTimer_WritePeriod(_daq_cfg.daq_interval_ms - 1);
            UnixTimer_WriteCounter(_daq_cfg.daq_interval_ms - 1);
            UnixTimer_Start();
            //
        }
    } break;
    default:
        break;
    }
}

extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler(void);
#define CORTEX_INTERRUPT_BASE          (16)

void setupFreeRTOS()
{
    /* Handler for Cortex Supervisor Call (SVC, formerly SWI) - address 11 */
    CyIntSetSysVector( CORTEX_INTERRUPT_BASE + SVCall_IRQn,
        (cyisraddress)vPortSVCHandler );
 
    /* Handler for Cortex PendSV Call - address 14 */
	CyIntSetSysVector( CORTEX_INTERRUPT_BASE + PendSV_IRQn,
        (cyisraddress)xPortPendSVHandler );    
 
    /* Handler for Cortex SYSTICK - address 15 */
	CyIntSetSysVector( CORTEX_INTERRUPT_BASE + SysTick_IRQn,
        (cyisraddress)xPortSysTickHandler );
}

void LED_Task(void *arg)
{
    (void)arg;
 
        while(1) {
        vTaskDelay(500);
        }
}
 

int main()
{
    CyGlobalIntEnable;
    setupFreeRTOS();
 
    /* Create LED task, which will control the intensity of the LEDs */
    xTaskCreate(
        LED_Task,       /* Task function */
        "LED Blink",    /* Task name (string) */
        200,            /* Task stack, allocated from heap */
        0,              /* No param passed to task function */
        1,              /* Low priority */
        0 );            /* Not using the task handle */
 
    vTaskStartScheduler();
    // _daq_cfg.daq_interval_ms = 500;
    ble_lib_init();
    bi2c_setup();
    ADC_Start();
    ADC_StartConvert();
    UnixTimerTCIRQ_StartEx(UnixTimerTCIRQHandler);
    UnixTimer_Start();

#if 0 // testcode
    {
        const struct pmod_api *_new_sensor_api = &k_sensor_spi_table[ePMOD_TYPE_ID_VL53L0X];
        if (_new_sensor_api != sensor_api) {
            if(sensor_api && sensor_api->exit){
                sensor_api->exit();
            }
            mem_lib_reset(eMEM_IRAM);
            _new_sensor_api->setup(ePMOD_TYPE_ID_VL53L0X, 0, 0);
            sensor_api = _new_sensor_api;
        }
        //
        _daq_state.sensor_type = ePMOD_TYPE_ID_VL53L0X;
        _daq_state.is_remote = true;
        _daq_state.state = eCS_SENSOR_STATE_RUNNING;
        _daq_state.err_code = 0;
        //
        UnixTimer_WritePeriod(100 - 1);
        UnixTimer_WriteCounter(100 - 1);
    }
#endif

    for (;;) {
        //
        CyBle_ProcessEvents();
        //
        if ((_daq_cfg.daq_duration_ms > 0) && (timer_ref_ms > _daq_cfg.daq_duration_ms)) {
            UnixTimer_Stop();
            _daq_state.state = eCS_SENSOR_STATE_PAUSED;
        }
        switch (_daq_state.state) {
        case eCS_SENSOR_STATE_IDLE: {
        } break;
        case eCS_SENSOR_STATE_RUNNING:
            if (sensor_api) {
                if (sensor_api->push_sample(&push_sample) != 0) {
                    // should we automatically pause collection in case of any error or keep retry
                    // _daq_state.sensor_type = ePMOD_TYPE_ID_NULL;
                    // _daq_state.state = eCS_SENSOR_STATE_PAUSED;
                }
            }
        case eCS_SENSOR_STATE_PAUSED:
            if (sensor_api) {
                push_sample &= !(_daq_state.state == eCS_SENSOR_STATE_PAUSED);
                if (sensor_api->send_sample(CyBle_GetState() == CYBLE_STATE_CONNECTED) != 0) {
                    _daq_state.state = eCS_SENSOR_STATE_IDLE;
                }
            }
            break;
        }
        //
        ble_lib_send_pending_frame();
    }
}
