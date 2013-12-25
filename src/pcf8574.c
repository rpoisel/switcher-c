#include <stdlib.h>
#include <stdio.h>

#include "i2c.h"
#include "i2c_io.h"
#include "pcf8574.h"

static int pcf8574_write(int fh, uint8_t pAddress, uint32_t pValue,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf,
        int buf_size);
static uint32_t pcf8574_init(void* init_value);
static uint32_t pcf8574_read(void);

static i2c_drv handle = {
    .init = &pcf8574_init, 
    .read = &pcf8574_read,
    .write = &pcf8574_write
};

/* write IOs */
static int pcf8574_write(int fh, uint8_t pAddress, uint32_t pValue,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf,
        int buf_size)
{
    /* I2C write */
    return i2c_write(fh, pAddress, pValue, cb_error, buf, buf_size);
}

static uint32_t pcf8574_init(void* init_value)
{
    /* nothing to do here */
    return 0;
}

/* read IOs */
static uint32_t pcf8574_read(void)
{
    /* I2C read */
    return 0;
}

i2c_drv* get_pcf8574_drv()
{
    return &handle;
}

