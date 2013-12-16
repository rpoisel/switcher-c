#include <stdlib.h>

#include "i2c_io.h"
#include "pcf8574.h"

static void write(uint8_t pAddress, uint32_t pValue);
static uint32_t init(void* pInitValue);
static uint32_t read(void);

static i2c_drv sHandle = { &init, &read, &write };

/* write IOs */
static void write(uint8_t pAddress, uint32_t pValue)
{
    /* I2C write */
}

static uint32_t init(void* pInitValue)
{
    /* nothing to do here */
    return 0;
}

/* read IOs */
static uint32_t read(void)
{
    /* I2C read */
    return 0;
}

i2c_drv* open_pcf8574_drv()
{
    return &sHandle;
}

void close_pcf8574_drv(i2c_drv* pHandle)
{
    /* nothing to do here */
}

