#include <stdlib.h>
#include <stdio.h>

#include "io_i2c.h"
#include "io.h"
#include "pcf8574.h"

static int pcf8574_write(int fh, uint8_t address, const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int pcf8574_read(int fh, uint8_t address, uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

static io_drv handle =
{ .init = NULL, .read = &pcf8574_read, .write = &pcf8574_write, .deinit = NULL };

/* write IOs */
static int pcf8574_write(int fh, uint8_t address, const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t states = (uint8_t) (*value);
	return i2c_write(fh, address, &states, sizeof(states), cb_error, buf_msg,
			buf_size_msg);
}

/* read IOs */
static int pcf8574_read(int fh, uint8_t address, uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t states = 0;
	int result = i2c_read(fh, address, &states, sizeof(states), cb_error,
			buf_msg, buf_size_msg);
	(*value) = states;
	return result;
}

io_drv* get_pcf8574_drv()
{
	return &handle;
}

