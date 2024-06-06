/**
 * @brief
 *
 */
#pragma once

#include <stdint.h>

extern void MLX90640_I2CInit(void);
extern int MLX90640_I2CGeneralReset(void *slave);
extern int MLX90640_I2CRead(void *slave, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
extern int MLX90640_I2CWrite(void *slave, uint16_t writeAddress, uint16_t data);
extern void MLX90640_I2CFreqSet(int freq);