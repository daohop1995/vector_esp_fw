/**
 * @file MLX90640.c
 * @author Anuj Pathak (anuj@biii.in)
 * @brief
 * @version 0.1
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020
 *
 */
#include "mlx90640.h"
#include "mlx90640_api.h"
#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"

#define FPS0p5HZ 0x00
#define FPS1HZ   0x01
#define FPS2HZ   0x02
#define FPS4HZ   0x03
#define FPS8HZ   0x04
#define FPS16HZ  0x05
#define FPS32HZ  0x06
#define TA_SHIFT 8 // Default shift for MLX90640 in open air

PMOD_SETUP_API(mlx90640)
{
    int err;
    if ((err = MLX90640_SetRefreshRate(inst->cfg.com, FPS32HZ)) == 0) {
        err = MLX90640_SetChessMode(inst->cfg.com);
    }
    return err;
}

PMOD_POLL_API(mlx90640)
{
    struct mlx90640_sample_value sample;
    switch (((struct mlx90640_stm *)inst->cfg.self)->data_to_capture) {
    case MLX90640_EEPARAM:
        if (MLX90640_DumpEE(inst->cfg.com, sample.eeMLX90640) == 0) {
            sample.ee_id = MLX90640_EEPARAM;
            ((struct mlx90640_stm *)inst->cfg.self)->data_to_capture = MLX90640_FRAME_0;
            return 0;
        }
        break;
    case MLX90640_FRAME_0:
    case MLX90640_FRAME_1:
        if (MLX90640_GetFrameData(inst->cfg.com, (uint16_t *)sample.frame) == ((struct mlx90640_stm *)inst->cfg.self)->data_to_capture) {
            ((struct mlx90640_stm *)inst->cfg.self)->data_to_capture ^= MLX90640_FRAME_1;
            return 0;
        }
        break;
    }
    return -ENODEV;
}

PMOD_EXIT_API(mlx90640)
{
    return 0;
}

const struct bi2c_slave mlx90640_com_cfg = {
    .id = BI2C_0,
    .addr = ePMOD_I2C_ADDR_MLS90640,
};

struct mlx90640_stm mlx90640_self_cfg;

PMOD_INST_DEFINE(mlx90640);
