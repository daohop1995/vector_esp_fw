/**
 * @file MLX90640.h
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-25
 * @ref https://github.com/imliubo/MLX90640-With-STM32
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include "hal/bi2c/bi2c.h"

enum mlx90640_data_type {
    MLX90640_FRAME_0,
    MLX90640_FRAME_1,
    MLX90640_EEPARAM,
};

struct mlx90640_stm {
    enum mlx90640_data_type data_to_capture;
    uint8_t rsv;
};

struct mlx90640_sample_value {
    union {
        struct {
            uint16_t eeMLX90640[832];
            uint16_t rsv;
            uint16_t ee_id;
        };
        uint16_t frame[834];
    };
};
