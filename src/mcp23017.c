#include <stdlib.h>
#include <stdio.h>

#include "io_i2c.h"
#include "io.h"
#include "mcp23017.h"

static uint32_t mcp23017_init(io_bus* bus, io_dev* dev);
static int mcp23017_write(io_bus* bus, io_dev* dev, const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int mcp23017_read(io_bus* bus, io_dev* dev, uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static uint32_t mcp23017_deinit(io_bus* bus, io_dev* dev);

static dev_drv handle =
{ .init = mcp23017_init, .read = &mcp23017_read, .write = &mcp23017_write, .deinit = mcp23017_deinit };

static uint32_t mcp23017_init(io_bus* bus, io_dev* dev)
{
	return 0;
}

/* write IOs */
static int mcp23017_write(io_bus *bus, io_dev* dev,
		const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	/* check whether device has been opened in write mode and configure accordingly if necessary */
#if 0
	uint8_t states = *value;
	return i2c_write(fh, address, &states, sizeof(states), cb_error, buf_msg,
			buf_size_msg);
#else
	return 0;
#endif
}

/* read IOs */
static int mcp23017_read(io_bus* bus, io_dev* dev,
		uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	/* check whether device has been opened in read mode and configure accordingly if necessary */
#if 0
	uint8_t states = 0;
	int result = i2c_read(fh, address, &states, sizeof(states), cb_error,
			buf_msg, buf_size_msg);
	(*value) = states;
#else
	int result = 0;
#endif
	return result;
}

dev_drv* get_mcp23017_drv()
{
	return &handle;
}

static uint32_t mcp23017_deinit(io_bus* bus, io_dev* dev)
{
	return 0;
}
