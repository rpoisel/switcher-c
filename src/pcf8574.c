#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c.h"
#include "i2c_io.h"
#include "pcf8574.h"

    /* write IOs */
static int pcf8574_write(int fh, uint8_t address, const uint32_t* value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);
static uint32_t pcf8574_init(void* init_value);
static int pcf8574_read(int fh, uint8_t address, uint32_t* value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);

static i2c_drv handle = {
    .init = &pcf8574_init, 
    .read = &pcf8574_read,
    .write = &pcf8574_write
};

/* write IOs */
static int pcf8574_write(int fh, uint8_t address, const uint32_t* value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg)
{
    uint8_t states = (uint8_t)(*value);
    return i2c_ioop(fh, address, &states, sizeof(states), write_unconst, cb_error, buf_msg, buf_size_msg);
}

static uint32_t pcf8574_init(void* init_value)
{
    /* nothing to do here */
    return 0;
}

/* read IOs */
static int pcf8574_read(int fh, uint8_t address, uint32_t* value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg)
{
    uint8_t states = 0;
    int result = i2c_ioop(fh, address, &states, sizeof(states), read, cb_error, buf_msg, buf_size_msg);
    (*value) = states;
    return result;
}

i2c_drv* get_pcf8574_drv()
{
    return &handle;
}

