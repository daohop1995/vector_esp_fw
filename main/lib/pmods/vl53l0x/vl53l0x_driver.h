#ifndef VL53L0X_h
#define VL53L0X_h

#include <stdint.h>
#include <stdbool.h>
#include "hal/bi2c/bi2c.h"
#include "hal/butils.h"
#include "vl53l0x.h"

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define ADDRESS_DEFAULT      0b0101001
#define mcVLX53L0X_MSB_FIRST 1
#define DEFULT_TIMEOUT       1000

// Decode VCSEL (vertical cavity surface emitting laser) pulse period in PCLKs
// from register value
// based on VL53L0X_decode_vcsel_period()
#define decodeVcselPeriod(reg_val) (((reg_val) + 1) << 1)

// Encode VCSEL pulse period register value from period in PCLKs
// based on VL53L0X_encode_vcsel_period()
#define encodeVcselPeriod(period_pclks) (((period_pclks) >> 1) - 1)

// Calculate macro period in *nanoseconds* from VCSEL period in PCLKs
// based on VL53L0X_calc_macro_period_ps()
// PLL_period_ps = 1655; macro_period_vclks = 2304
#define calcMacroPeriod(vcsel_period_pclks) ((((uint32_t)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)

#define WaitForVL53L0XConditionOrTimeout(x, timeout, ontimeout) \
    {                                                           \
        int i = (timeout);                                      \
        for (; (i > 0) && (!(x)); i--) {                        \
            taskYIELD();                                        \
        }                                                       \
        if (i == 0) {                                           \
            ontimeout                                           \
        }                                                       \
    }

#define VL53L0X_setAddress(sensor_stm, new_addr)                                       \
    bi2c_write_reg_u8(sensor_stm->address, I2C_SLAVE_DEVICE_ADDRESS, new_addr & 0x7F); \
    sensor_stm->address = new_addr

#define VL53L0X_getAddress(sensor_stm) sensor_stm->address

enum regAddr {
    SYSRANGE_START = 0x00,

    SYSTEM_THRESH_HIGH = 0x0C,
    SYSTEM_THRESH_LOW = 0x0E,

    SYSTEM_SEQUENCE_CONFIG = 0x01,
    SYSTEM_RANGE_CONFIG = 0x09,
    SYSTEM_INTERMEASUREMENT_PERIOD = 0x04,

    SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0A,

    GPIO_HV_MUX_ACTIVE_HIGH = 0x84,

    SYSTEM_INTERRUPT_CLEAR = 0x0B,

    RESULT_INTERRUPT_STATUS = 0x13,
    RESULT_RANGE_STATUS = 0x14,

    RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN = 0xBC,
    RESULT_CORE_RANGING_TOTAL_EVENTS_RTN = 0xC0,
    RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF = 0xD0,
    RESULT_CORE_RANGING_TOTAL_EVENTS_REF = 0xD4,
    RESULT_PEAK_SIGNAL_RATE_REF = 0xB6,

    ALGO_PART_TO_PART_RANGE_OFFSET_MM = 0x28,

    I2C_SLAVE_DEVICE_ADDRESS = 0x8A,

    MSRC_CONFIG_CONTROL = 0x60,

    PRE_RANGE_CONFIG_MIN_SNR = 0x27,
    PRE_RANGE_CONFIG_VALID_PHASE_LOW = 0x56,
    PRE_RANGE_CONFIG_VALID_PHASE_HIGH = 0x57,
    PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT = 0x64,

    FINAL_RANGE_CONFIG_MIN_SNR = 0x67,
    FINAL_RANGE_CONFIG_VALID_PHASE_LOW = 0x47,
    FINAL_RANGE_CONFIG_VALID_PHASE_HIGH = 0x48,
    FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT = 0x44,

    PRE_RANGE_CONFIG_SIGMA_THRESH_HI = 0x61,
    PRE_RANGE_CONFIG_SIGMA_THRESH_LO = 0x62,

    PRE_RANGE_CONFIG_VCSEL_PERIOD = 0x50,
    PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x51,
    PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x52,

    SYSTEM_HISTOGRAM_BIN = 0x81,
    HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT = 0x33,
    HISTOGRAM_CONFIG_READOUT_CTRL = 0x55,

    FINAL_RANGE_CONFIG_VCSEL_PERIOD = 0x70,
    FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI = 0x71,
    FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO = 0x72,
    CROSSTALK_COMPENSATION_PEAK_RATE_MCPS = 0x20,

    MSRC_CONFIG_TIMEOUT_MACROP = 0x46,

    SOFT_RESET_GO2_SOFT_RESET_N = 0xBF,
    IDENTIFICATION_MODEL_ID = 0xC0,
    IDENTIFICATION_REVISION_ID = 0xC2,

    OSC_CALIBRATE_VAL = 0xF8,

    GLOBAL_CONFIG_VCSEL_WIDTH = 0x32,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_0 = 0xB0,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_1 = 0xB1,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_2 = 0xB2,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_3 = 0xB3,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_4 = 0xB4,
    GLOBAL_CONFIG_SPAD_ENABLES_REF_5 = 0xB5,

    GLOBAL_CONFIG_REF_EN_START_SELECT = 0xB6,
    DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD = 0x4E,
    DYNAMIC_SPAD_REF_EN_START_OFFSET = 0x4F,
    POWER_MANAGEMENT_GO1_POWER_FORCE = 0x80,

    VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV = 0x89,

    ALGO_PHASECAL_LIM = 0x30,
    ALGO_PHASECAL_CONFIG_TIMEOUT = 0x30,
};

enum vcselPeriodType {
    VcselPeriodPreRange,
    VcselPeriodFinalRange
};

bool VL53L0X_setSignalRateLimit(struct vl53l0x_stm *sensor_stm, float limit_Mcps);
float VL53L0X_getSignalRateLimit(struct vl53l0x_stm *sensor_stm);
bool VL53L0X_setMeasurementTimingBudget(struct vl53l0x_stm *sensor_stm, uint32_t budget_us);
uint32_t VL53L0X_getMeasurementTimingBudget(struct vl53l0x_stm *sensor_stm);
bool VL53L0X_setVcselPulsePeriod(struct vl53l0x_stm *sensor_stm, enum vcselPeriodType type, uint8_t period_pclks);
uint16_t VL53L0X_getVcselPulsePeriod(struct vl53l0x_stm *sensor_stm, enum vcselPeriodType type);

// bool init(bool io_2v8 = true); remove default argument cuz it is c++ feature
bool VL53L0X_init(struct vl53l0x_stm *sensor_stm, bool io_2v8);
void VL53L0X_startContinuous(struct vl53l0x_stm *sensor_stm, uint32_t period_ms);
void VL53L0X_stopContinuous(struct vl53l0x_stm *sensor_stm);
uint16_t VL53L0X_readRangeContinuousMillimeters(struct vl53l0x_stm *sensor_stm);
uint16_t VL53L0X_readRangeSingleMillimeters(struct vl53l0x_stm *sensor_stm);
bool VL53L0X_timeoutOccurred(struct vl53l0x_stm *sensor_stm);

struct SequenceStepEnables {
    bool tcc, msrc, dss, pre_range, final_range;
};

struct SequenceStepTimeouts {
    uint16_t pre_range_vcsel_period_pclks, final_range_vcsel_period_pclks;
    uint16_t msrc_dss_tcc_mclks, pre_range_mclks, final_range_mclks;
    uint32_t msrc_dss_tcc_us, pre_range_us, final_range_us;
};

#endif
