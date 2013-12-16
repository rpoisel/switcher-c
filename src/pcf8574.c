#include <stdlib.h>
#include <stdio.h>

#include "i2c_io.h"
#include "pcf8574.h"

static void pcf8574_write(uint8_t pAddress, uint32_t pValue);
static uint32_t pcf8574_init(void* pInitValue);
static uint32_t pcf8574_read(void);

static i2c_drv sHandle = {
    .init = &pcf8574_init, 
    .read = &pcf8574_read,
    .write = &pcf8574_write
};

/* write IOs */
static void pcf8574_write(uint8_t pAddress, uint32_t pValue)
{
    /* I2C write */
    printf("Writing. Address: 0x%02X, Value: 0x%02X\n", pAddress, pValue);
}

static uint32_t pcf8574_init(void* pInitValue)
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
    return &sHandle;
}

void close_pcf8574_drv(i2c_drv* pHandle)
{
    /* nothing to do here */
}

