/**
 * @file pmods.c
 * @author Anuj Pathak (anuj@croxel.com)
 * @brief
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "pmods.h"
#include "lib/pmods/pmods.h"
#include "adxl346/adxl346.h"
#include "ina219/ina219.h"
#include "kmx61/kmx61.h"
#include "mlx90640/mlx90640.h"
#include "si7006/si7006.h"
#include "vl53l0x/vl53l0x.h"

extern PMOD_INST_DECLARE(si7006);
extern PMOD_INST_DECLARE(ina219);
extern PMOD_INST_DECLARE(kmx61);
extern PMOD_INST_DECLARE(adxl346);
extern PMOD_INST_DECLARE(vl53l0x);
extern PMOD_INST_DECLARE(mlx90640);

const struct pmod_inst* pmod_instances[ePMOD_TYPE_ID_MAX] = {
    [ePMOD_TYPE_ID_SI7006] = &PMOD_INST(si7006),
    [ePMOD_TYPE_ID_INA219] = &PMOD_INST(ina219),
    [ePMOD_TYPE_ID_KMX61] = &PMOD_INST(kmx61),
    [ePMOD_TYPE_ID_ADXL346] = &PMOD_INST(adxl346),
    [ePMOD_TYPE_ID_VL53L0X] = &PMOD_INST(vl53l0x),
    [ePMOD_TYPE_ID_MLS90640] = &PMOD_INST(mlx90640),
};