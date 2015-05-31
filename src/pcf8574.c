#include <stdlib.h>
#include <stdio.h>

#include <io_i2c.h>
#include <io.h>
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
	return bus->drv_handle->write(bus, dev, &states, sizeof(states), cb_error,
			buf_msg, buf_size_msg);
}

/* read IOs */
static int pcf8574_read(io_bus* bus, io_dev* dev, value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t states = 0;
	int result = bus->drv_handle->read(bus, dev, &states, sizeof(states),
			cb_error, buf_msg, buf_size_msg);
	(*value) = states;
	return result;
}

dev_drv* get_pcf8574_drv()
{
	return &handle;
}

