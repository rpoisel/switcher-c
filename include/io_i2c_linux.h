#ifndef IO_I2C_LINUX_H_
#define IO_I2C_LINUX_H_ 1

#include <stdint.h>
#include <sys/types.h>

#include "external/linux/i2c-dev.h"

#include "io_fwd.h"

#define I2C_DEV_LEN 32

typedef struct i2c_linux_bus_data_
{
	char device[I2C_DEV_LEN];
	int fh;
} i2c_linux_bus_data;

typedef struct i2c_linux_dev_data_
{
	uint8_t address;
} i2c_linux_dev_data;

bus_drv* get_i2c_linux_drv();

extern int i2c_linux_write_byte(io_bus* bus, io_dev* dev, uint8_t value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

extern int i2c_linux_read_byte(io_bus* bus, io_dev* dev, uint8_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

extern int i2c_linux_write_byte_data(io_bus* bus, io_dev* dev, uint8_t command,
		uint8_t value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

extern int i2c_linux_read_byte_data(io_bus* bus, io_dev* dev, uint8_t command,
		uint8_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

#endif /* IO_I2C_LINUX_H_ */
