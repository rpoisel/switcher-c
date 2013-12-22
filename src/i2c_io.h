#ifndef __I2C_IO_H__
#define __I2C_IO_H__ 1

#include <stdint.h>

#define MAX_I2C_BUSSES 8
#define MAX_IO_DEVICES 128

/* for example: /dev/i2c-2 */
#define MAX_DEV_FILE_LEN 32

typedef enum
{
    CMD_NONE,
    CMD_WRITE,
    CMD_READ,
} i2c_cmd;

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
    unsigned num_busses;
    i2c_bus busses[MAX_I2C_BUSSES];
} i2c_config;

typedef struct
{
    unsigned idx_bus;
    unsigned idx_dev;
    unsigned value;
} i2c_data;

int perform_i2c_io(i2c_config* config, i2c_data* data, i2c_cmd cmd);

#endif
