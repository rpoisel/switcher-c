#ifndef __I2C_IO_H__
#define __I2C_IO_H__ 1

#include <stdint.h>

#include "i2c_io_fwd.h"

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
    uint32_t (*init)(void* init_value);

    /* read IOs */
    int (*read)(int fh, uint8_t address, uint32_t* value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);

    /* write IOs */
    int (*write)(int fh, uint8_t address, const uint32_t* value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);
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

struct i2c_cfg
{
    unsigned num_busses;
    i2c_bus busses[MAX_I2C_BUSSES];
};

typedef struct
{
    unsigned idx_bus;
    unsigned idx_dev;
    uint32_t value;
} i2c_data;

int perform_i2c_io(i2c_config* config, i2c_data* data, i2c_cmd cmd,
        int (*cb_success)(i2c_data* data, char* buf, int buf_size),
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf,
        int buf_size);

#endif
