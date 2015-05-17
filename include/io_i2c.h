#ifndef IO_I2C_H_
#define IO_I2C_H_ 1

#include <stdint.h>
#include <sys/types.h>

#include "io_fwd.h"

#define I2C_DEV_LEN 32

typedef struct i2c_bus_data_
{
	char device[I2C_DEV_LEN];
	int fh;
} i2c_bus_data;

typedef struct i2c_dev_data_
{
	uint8_t address;
} i2c_dev_data;

bus_drv* get_i2c_drv();

#endif /* IO_I2C_H_ */
