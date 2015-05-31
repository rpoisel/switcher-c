#include <stdlib.h>
#include <stdio.h>

#include "io_i2c.h"
#include "io.h"
#include "mcp23017.h"

typedef enum _mcp23017_io_state
{
	MCP23017_NONE, MCP23017_READ, MCP23017_WRITE
} mcp23017_io_state;

typedef struct mcp23017_data_
{
	mcp23017_io_state io_state; /* read or write */
} mcp23017_data;

static uint32_t mcp23017_init(io_bus* bus, io_dev* dev);
static int mcp23017_write(io_bus* bus, io_dev* dev, const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int mcp23017_read(io_bus* bus, io_dev* dev, value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static uint32_t mcp23017_deinit(io_bus* bus, io_dev* dev);

static dev_drv handle =
{ .init = mcp23017_init, .read = &mcp23017_read, .write = &mcp23017_write,
		.deinit = mcp23017_deinit };

static uint32_t mcp23017_init(io_bus* bus, io_dev* dev)
{
	mcp23017_data* dev_data = (dev->dev_data = malloc(sizeof(mcp23017_data)));
	if (NULL == dev->dev_data)
	{
		return 1;
	}
	dev_data->io_state = MCP23017_NONE;
	return 0;
}

/* write IOs */
static int mcp23017_write(io_bus *bus, io_dev* dev, const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	/* check whether device has been opened in write mode and configure accordingly if necessary */
	uint16_t states = *value;
	uint8_t i2c_cmds[2] =
	{ 0 };
	ssize_t data_written = 0;
	mcp23017_data* dev_data = dev->dev_data;
	if (dev_data->io_state != MCP23017_WRITE)
	{
		i2c_cmds[0] = 0x00; /* IODIRA */
		i2c_cmds[1] = 0x00; /* all outputs */
		data_written = bus->drv_handle->write(bus, dev, i2c_cmds,
				sizeof(i2c_cmds), cb_error, buf_msg, buf_size_msg);
		if (sizeof(i2c_cmds) == data_written)
		{
			i2c_cmds[0] = 0x01; /* IODIRB */
			i2c_cmds[1] = 0x00; /* all outputs */
			data_written = bus->drv_handle->write(bus, dev, i2c_cmds,
					sizeof(i2c_cmds), cb_error, buf_msg, buf_size_msg);
			if (sizeof(i2c_cmds) == data_written)
			{
				dev_data->io_state = MCP23017_WRITE;
			}
		}
	}
	if (MCP23017_WRITE == dev_data->io_state)
	{
		data_written = 0;
		i2c_cmds[0] = 0x14; /* OLATA */
		i2c_cmds[1] = states & 0x00FF; /* LSB */
		data_written += bus->drv_handle->write(bus, dev, i2c_cmds, sizeof(i2c_cmds),
				cb_error, buf_msg, buf_size_msg);
		i2c_cmds[0] = 0x15; /* OLATB */
		i2c_cmds[1] = (states & 0xFF00) >> 8; /* MSB */
		data_written += bus->drv_handle->write(bus, dev, i2c_cmds, sizeof(i2c_cmds),
				cb_error, buf_msg, buf_size_msg);
		return data_written;
	}
	return 0;
}

/* read IOs */
static int mcp23017_read(io_bus* bus, io_dev* dev, value_t* value,
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
	if (NULL != dev->dev_data)
	{
		free(dev->dev_data);
		dev->dev_data = NULL;
	}
	return 0;
}
