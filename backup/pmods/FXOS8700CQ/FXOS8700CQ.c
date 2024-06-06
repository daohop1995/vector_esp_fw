/**
 * @file FXOS8700CQ.c
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-24
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "FXOS8700CQ.h"

static struct
{
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
    } mag;
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
    } accl;
} data;

// function configures FXOS8700CQ combination accelerometer and magnetometer sensor
int fxos8700cq_setup(uint8_t addr, void *settings, uint16_t settings_len)
{
    // read and check the FXOS8700CQ WHOAMI register
    uint8_t who_am_i = bi2c_read_reg_u8(addr, FXOS8700CQ_WHOAMI);
    if ( who_am_i == FXOS8700CQ_WHOAMI_VAL)
    {
        // write 0000 0000 = 0x00 to accelerometer control register 1 to place FXOS8700CQ into standby
        // [7-1] = 0000 000
        // [0]: active=0
        bi2c_write_reg_u8(addr, FXOS8700CQ_CTRL_REG1, 0x00);
        // write 0001 1111 = 0x1F to magnetometer control register 1
        // [7]: m_acal=0: auto calibration disabled
        // [6]: m_rst=0: no one-shot magnetic reset
        // [5]: m_ost=0: no one-shot magnetic measurement
        // [4-2]: m_os=111=7: 8x oversampling (for 200Hz) to reduce magnetometer noise
        // [1-0]: m_hms=11=3: select hybrid mode with accel and  magnetometer active
        bi2c_write_reg_u8(addr, FXOS8700CQ_M_CTRL_REG1, 0x1F);
        // write 0010 0000 = 0x20 to magnetometer control register 2
        // [7]: reserved
        // [6]: reserved
        // [5]: hyb_autoinc_mode=1 to map the magnetometer registers to follow the
        // accelerometer registers
        // [4]: m_maxmin_dis=0 to retain default min/max latching even though not used
        // [3]: m_maxmin_dis_ths=0
        // [2]: m_maxmin_rst=0
        // [1-0]: m_rst_cnt=00 to enable magnetic reset each cycle
        bi2c_write_reg_u8(addr, FXOS8700CQ_M_CTRL_REG2, 0x20);
        // write 0000 0001= 0x01 to XYZ_DATA_CFG register
        // [7]: reserved
        // [6]: reserved
        // [5]: reserved
        // [4]: hpf_out=0
        // [3]: reserved
        // [2]: reserved
        // [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg / LSB
        bi2c_write_reg_u8(addr, FXOS8700CQ_XYZ_DATA_CFG, 0x01);
        // write 0000 1101 = 0x0D to accelerometer control register 1
        // [7-6]: aslp_rate=00
        // [5-3]: dr=001 for 200Hz data rate (when in hybrid mode)
        // [2]: lnoise=1 for low noise mode
        // [1]: f_read=0 for normal 16 bit reads
        // [0]: active=1 to take the part out of standby and enable sampling
        bi2c_write_reg_u8(addr, FXOS8700CQ_CTRL_REG1, 0x0D);
        // normal return
        return true;
    }
    return false;
}

// read status and the three channels of accelerometer and magnetometer data from FXOS8700CQ (13 bytes)
int fxos8700cq_loop(bool push_sample, bool send_sample)
{
    uint8_t buffer[FXOS8700CQ_READ_LEN]; // read buffer
    // read FXOS8700CQ_READ_LEN=13 bytes (status byte and the six channels of data)
    bi2c_read_bytes(addr, (uint8_t[]){FXOS8700CQ_STATUS}, 1, buffer, FXOS8700CQ_READ_LEN);
    // copy the 14 bit accelerometer byte data into 16 bit words
    data.accl.x = (int16_t)(((buffer[1] << 8) | buffer[2])) >> 2;
    data.accl.y = (int16_t)(((buffer[3] << 8) | buffer[4])) >> 2;
    data.accl.z = (int16_t)(((buffer[5] << 8) | buffer[6])) >> 2;
    // copy the magnetometer byte data into 16 bit words
    data.mag.x = (buffer[7] << 8) | buffer[8];
    data.mag.y = (buffer[9] << 8) | buffer[10];
    data.mag.z = (buffer[11] << 8) | buffer[12];
    //
    *buf = (uint8_t*)&data;
    *buf_len = sizeof(data);
	return true;
}