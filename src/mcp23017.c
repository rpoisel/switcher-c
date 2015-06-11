#include <io_i2c_linux.h>
#include <stdlib.h>
#include <stdio.h>

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

static void mcp23017_configure_io(io_bus *bus, io_dev *dev,
		mcp23017_data* dev_data, mcp23017_io_state desired_state,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t io_states = 0x00 /* all outputs */;
	dev_data->io_state = MCP23017_NONE;

	if (desired_state != MCP23017_WRITE && desired_state != MCP23017_READ)
	{
		return;
	}

	io_states =
			desired_state == MCP23017_WRITE ?
					0x00 /* all outputs */: 0xFF /* all inputs */;

	if (i2c_linux_write_byte_data(bus, dev, 0x00 /* IODIRA */, io_states,
			cb_error, buf_msg, buf_size_msg) == 0)
	{
		if (i2c_linux_write_byte_data(bus, dev, 0x01 /* IODIRB */, io_states,
				cb_error, buf_msg, buf_size_msg) == 0)
		{
			dev_data->io_state = desired_state;
		}
	}

}

/* write IOs */
static int mcp23017_write(io_bus *bus, io_dev* dev, const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint16_t states = *value;
	mcp23017_data* dev_data = dev->dev_data;
	if (dev_data->io_state != MCP23017_WRITE)
	{
		mcp23017_configure_io(bus, dev, dev_data, MCP23017_WRITE, cb_error,
				buf_msg, buf_size_msg);
	}
	if (MCP23017_WRITE == dev_data->io_state)
	{
		if (i2c_linux_write_byte_data(bus, dev, 0x14 /* OLATA */,
				((uint8_t) states & 0x00FF), cb_error, buf_msg, buf_size_msg)
				== 0)
		{
			if (i2c_linux_write_byte_data(bus, dev, 0x15 /* OLATB */,
					((uint8_t) ((states & 0xFF00) >> 8)), cb_error, buf_msg,
					buf_size_msg) == 0)
			{
				return 1;
			}
		}
	}
	else
	{
		cb_error("Could not configure given MCP23017 in write mode", buf_msg,
				buf_size_msg);
	}
	return 0;
}

/* read IOs */
static int mcp23017_read(io_bus* bus, io_dev* dev, value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t states_a = 0x00;
	uint8_t states_b = 0x00;
	mcp23017_data* dev_data = dev->dev_data;

	if (dev_data->io_state != MCP23017_READ)
	{
		mcp23017_configure_io(bus, dev, dev_data, MCP23017_READ, cb_error,
				buf_msg, buf_size_msg);
	}
	if (MCP23017_READ == dev_data->io_state)
	{
		if (i2c_linux_read_byte_data(bus, dev, 0x12 /* GPIOA */, &states_a,
				cb_error, buf_msg, buf_size_msg) == 0)
		{
			if (i2c_linux_read_byte_data(bus, dev, 0x13 /* GPIOB */, &states_b,
					cb_error, buf_msg, buf_size_msg) == 0)
			{
				(*value) = 0x00;
				(*value) |= states_a;
				(*value) |= (states_b << 8);
				return 1;
			}
		}
	}
	else
	{
		cb_error("Could not configure givem MCP23017 in read mode", buf_msg,
				buf_size_msg);
	}
	return 0;
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
