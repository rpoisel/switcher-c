#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

#include <trace.h>
#include <io.h>
#include <io_i2c_linux.h>

#define BUF_LEN 256

static int i2c_linux_set_addr(io_bus* bus, io_dev* dev,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static void* get_i2c_linux_bus_data(const char* parameter);
static void* get_i2c_linux_dev_data(const char* parameter);
static void discard_i2c_linux_bus_data(void* bus_data);
static void discard_i2c_linux_bus_dev_data(void* bus_dev_data);
static int i2c_linux_open(io_bus* bus);
static int i2c_linux_close(io_bus* bus);

static bus_drv handle =
{ .get_bus_data = &get_i2c_linux_bus_data, .get_bus_dev_data =
		&get_i2c_linux_dev_data, .open = &i2c_linux_open, .close =
		&i2c_linux_close, .discard_bus_data = &discard_i2c_linux_bus_data,
		.discard_bus_dev_data = &discard_i2c_linux_bus_dev_data };

bus_drv* get_i2c_linux_drv()
{
	return &handle;
}

static int i2c_linux_set_addr(io_bus* bus, io_dev* dev,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	char error_msg[BUF_LEN] =
	{ '\0' };
	i2c_linux_bus_data *bus_data = bus->bus_data;
	i2c_linux_dev_data *dev_bus_data = dev->dev_bus_data;

	/* set slave address */
	if (ioctl(bus_data->fh, I2C_SLAVE, dev_bus_data->address) < 0)
	{
		snprintf(error_msg, BUF_LEN, "ioctl error: %s", strerror(errno));
		if (cb_error != NULL)
		{
			cb_error(error_msg, buf_msg, buf_size_msg);
		}
		return -1;
	}
	return 0;
}

int i2c_linux_write_byte(io_bus* bus, io_dev* dev, uint8_t value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	i2c_linux_bus_data *bus_data = bus->bus_data;
	i2c_linux_set_addr(bus, dev, cb_error, buf_msg, buf_size_msg);
	return i2c_smbus_write_byte(bus_data->fh, value);
}

int i2c_linux_read_byte(io_bus* bus, io_dev* dev, uint8_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	int32_t read_data = -1;
	i2c_linux_bus_data *bus_data = bus->bus_data;
	i2c_linux_set_addr(bus, dev, cb_error, buf_msg, buf_size_msg);
	read_data = i2c_smbus_read_byte(bus_data->fh);
	if (read_data < 0)
	{
		return -1;
	}
	*value = (uint8_t) read_data;
	return 0;
}

int i2c_linux_write_byte_data(io_bus* bus, io_dev* dev, uint8_t command,
		uint8_t value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	i2c_linux_bus_data *bus_data = bus->bus_data;
	i2c_linux_set_addr(bus, dev, cb_error, buf_msg, buf_size_msg);
	return i2c_smbus_write_byte_data(bus_data->fh, command, value);
}

int i2c_linux_read_byte_data(io_bus* bus, io_dev* dev, uint8_t command,
		uint8_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	int32_t read_data = -1;
	i2c_linux_bus_data *bus_data = bus->bus_data;
	i2c_linux_set_addr(bus, dev, cb_error, buf_msg, buf_size_msg);
	read_data = i2c_smbus_read_byte_data(bus_data->fh, command);
	if (read_data < 0)
	{
		return -1;
	}
	*value = (uint8_t) read_data;
	return 0;
}

static int i2c_linux_open(io_bus* bus)
{
	i2c_linux_bus_data* data = bus->bus_data;
	TRACE(printf("Opening I2C bus: %s\n", data->device));
	data->fh = open(data->device, O_RDWR);
	if (data->fh < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
static int i2c_linux_close(io_bus* bus)
{
	i2c_linux_bus_data* data = bus->bus_data;
	TRACE(printf("Closing I2C bus: %s\n", data->device));
	if (close(data->fh) < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static void* get_i2c_linux_bus_data(const char* parameter)
{
	i2c_linux_bus_data* data = malloc(sizeof(i2c_linux_bus_data));
	if (NULL != data)
	{
		strncpy(data->device, parameter, I2C_DEV_LEN);
		data->fh = -1;
	}
	return data;
}

static void* get_i2c_linux_dev_data(const char* parameter)
{
	i2c_linux_dev_data* data = malloc(sizeof(i2c_linux_dev_data));
	if (NULL != data)
	{
		data->address = strtol(parameter, NULL, 16);
	}
	return data;
}

static void discard_i2c_linux_bus_data(void* bus_data)
{
	free(bus_data);
}

static void discard_i2c_linux_bus_dev_data(void* bus_dev_data)
{
	free(bus_dev_data);
}
