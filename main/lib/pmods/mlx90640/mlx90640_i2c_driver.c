#include "hal/bi2c/bi2c.h"
#include "lib/pmods/pmods.h"

void MLX90640_I2CInit()
{
}

int MLX90640_I2CRead(void *slave, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    // switch big.endian to little.endian
    startAddress = __bswap16(startAddress);
    uint8_t *data8 = (uint8_t *)data;
    uint8_t tmp;
    int code = bi2c_read_bytes(slave, (uint8_t *)&startAddress, 2, data8, nMemAddressRead * 2);
    if (code == 0) {
        for (int i = 0; i < nMemAddressRead; i++) {
            tmp = data8[2 * i];
            data8[2 * i] = data8[2 * i + 1];
            data8[2 * i + 1] = tmp;
            // data[i] = _bswap16(data[i]);
        }
    }
    return code;
}

int MLX90640_I2CWrite(void *slave, uint16_t writeAddress, uint16_t data)
{
    // switch big.endian to little.endian
    writeAddress = __bswap16(writeAddress);
    data = __bswap16(data);
    return bi2c_write_bytes(slave, (uint8_t *)&writeAddress, 2, (uint8_t *)&data, 2);
}

int MLX90640_I2CGeneralReset(void *slave)
{
    return bi2c_write_byte(slave, 0, 0, 0x06);
}