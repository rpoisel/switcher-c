#ifndef __I2C_IO_H__
#define __I2C_IO_H__ 1

#include <stdint.h>

#define MAX_IO_DEVICES 128

typedef struct
{
    /* init device */
    uint32_t (*init)(void* pInitValue);

    /* read IOs */
    uint32_t (*read)(void);

    /* write IOs */
    void (*write)(uint8_t pAddres, uint32_t pValue);
} i2c_drv;

typedef struct
{
    uint8_t address;
    i2c_drv* drv_handle;
} i2c_io;

typedef struct
{
    int num_devices;
    i2c_io devices[MAX_IO_DEVICES];
} i2c_bus;

#endif
