#ifndef __I2C_IO_H__
#define __I2C_IO_H__ 1

#include <stdint.h>

#define MAX_I2C_BUSSES 8
#define MAX_IO_DEVICES 128
#define MAX_DEV_FILE_LEN 32

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
    unsigned num_devices;
    i2c_io devices[MAX_IO_DEVICES];
    char dev_file[MAX_DEV_FILE_LEN];
    int fh;
} i2c_bus;

typedef struct
{
    i2c_bus busses[MAX_I2C_BUSSES];
    unsigned num_busses;
} i2c_config;

#endif
