/**
 * @file i2csensor.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

enum pmod_type {
    ePMOD_TYPE_ID_NULL = 0,
    ePMOD_TYPE_ID_SI7006,
    ePMOD_TYPE_ID_INA219,
    ePMOD_TYPE_ID_KMX61,
    ePMOD_TYPE_ID_ADXL346,
    ePMOD_TYPE_ID_VL53L0X,
    ePMOD_TYPE_ID_MLS90640,
    //
    // ePMOD_TYPE_ID_ANALOG,
    // ePMOD_TYPE_ID_FXOS8700CQ,
    // ePMOD_TYPE_ID_ADS7142,
    //
    ePMOD_TYPE_ID_MAX,
};

enum pmod_com_inf {
    ePMOD_COM_I2C,
    ePMOD_COM_SPI,
    ePMOD_COM_I2S,
};

enum pmod_default_addr {
    ePMOD_I2C_ADDR_SI7006 = 0x40,
    ePMOD_I2C_ADDR_INA219 = 0x40,
    ePMOD_I2C_ADDR_KMX61 = 0x0E,
    ePMOD_I2C_ADDR_FXOS8700CQ = 0x1E,
    ePMOD_I2C_ADDR_VL53L0X = 0x29,
    ePMOD_I2C_ADDR_ADS7142,
    ePMOD_I2C_ADDR_MLS90640 = 0x33,
    ePMOD_I2C_ADDR_ADXL346 = 0x53,
};

struct pmod_inst;

typedef int (*pmod_on_sample_ready_f)(const void *sample, size_t sample_len, bool free_mem);

///
/// @brief function prototype to setup and initialize sensor,
/// after initialization it also prepares a settings buffer to send.
/// this information is utilized by flutter app for data processing
///
/// @return negative value corresponding to error codes
///         positive values represent number of bytes in sensor_settings needed to send
///
typedef int (*pmod_setup_f)(const struct pmod_inst *inst);

///
/// @brief function prototype to be defined by all the pmods.
/// this function should implement data gathering algorithm of the pmods and utilizes
///
/// @return negative value corresponding to error codes
///         positive values to denote number of bytes in @ref sensor_samples array ready to send
///
typedef int (*pmod_poll_f)(const struct pmod_inst *inst, pmod_on_sample_ready_f on_ready);

///
/// @brief
///
///
typedef int (*pmod_exit_f)(const struct pmod_inst *inst);

struct pmod_inst {
    struct {
        const void *com;
        const void *self;
    } cfg;
    struct {
        pmod_setup_f setup;
        pmod_poll_f poll;
        pmod_exit_f exit;
    } api;
};

extern const struct pmod_inst* pmod_instances[ePMOD_TYPE_ID_MAX];

#define PMOD_INST(name)         name##_pmod_inst
#define PMOD_INST_DECLARE(name) const struct pmod_inst PMOD_INST(name)
#define PMOD_INST_DEFINE(name)                                                    \
    PMOD_INST_DECLARE(name) = {                                                   \
        .cfg = {.com = &name##_com_cfg, .self = &name##_self_cfg},                \
        .api = {.setup = name##_setup, .poll = name##_poll, .exit = name##_exit}, \
    };

#define PMOD_SETUP_API(name) \
    int name##_setup(const struct pmod_inst *inst)

#define PMOD_POLL_API(name) \
    int name##_poll(const struct pmod_inst *inst, pmod_on_sample_ready_f on_ready)

#define PMOD_EXIT_API(name) \
    int name##_exit(const struct pmod_inst *inst)
