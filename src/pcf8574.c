#include <stdlib.h>
#include <stdio.h>

#include <io.h>
#include <io_i2c_linux.h>
#include <pcf8574.h>

static int pcf8574_write(io_bus* bus, io_dev* dev, const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int pcf8574_read(io_bus* bus, io_dev* dev, value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

static dev_drv handle =
{ .init = NULL, .read = &pcf8574_read, .write = &pcf8574_write, .deinit = NULL };

/* write IOs */
static int pcf8574_write(io_bus* bus, io_dev* dev, const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t states = (uint8_t) (*value);
	return i2c_linux_write_byte(bus, dev, states, cb_error, buf_msg,
			buf_size_msg) == 0 ? 1 : 0;
}

/* read IOs */
static int pcf8574_read(io_bus* bus, io_dev* dev, value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t states = 0;
	int result = i2c_linux_read_byte(bus, dev, &states, cb_error, buf_msg,
			buf_size_msg);
	(*value) = states;
	return result == 0 ? 1 : 0;
}

dev_drv* get_pcf8574_drv()
{
	return &handle;
}

