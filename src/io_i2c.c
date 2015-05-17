#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <linux/i2c-dev.h>

#include <trace.h>
#include <io_i2c.h>
#include <io.h>

#define BUF_LEN 256

static ssize_t write_unconst(int fd, void *buf, size_t count);

static int i2c_ioop(int fh, uint8_t address, void* buf, size_t buf_size,
		ssize_t (*ioop)(int fd, void* buf, size_t count),
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

static void* get_i2c_bus_data(const char* parameter);
static void* get_i2c_dev_data(const char* parameter);
static void discard_i2c_bus_data(void* bus_data);
static void discard_i2c_bus_dev_data(void* bus_dev_data);
static int i2c_read(io_bus* bus, io_dev* dev, void* buf, size_t buf_size,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int i2c_write(io_bus* bus, io_dev* dev, void* buf, size_t buf_size,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int i2c_open(io_bus* bus);
static int i2c_close(io_bus* bus);

static bus_drv handle =
{ .get_bus_data = &get_i2c_bus_data, .get_bus_dev_data = &get_i2c_dev_data,
		.open = &i2c_open, .write = &i2c_write, .read = &i2c_read, .close =
				&i2c_close, .discard_bus_data = &discard_i2c_bus_data,
		.discard_bus_dev_data = &discard_i2c_bus_dev_data };

static int i2c_write(io_bus* bus, io_dev* dev, void* buf, size_t buf_size,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	return i2c_ioop(((i2c_bus_data*) bus->bus_data)->fh,
			((i2c_dev_data*) dev->dev_bus_data)->address, buf, buf_size,
			write_unconst, cb_error, buf_msg, buf_size_msg);
}

static int i2c_read(io_bus* bus, io_dev* dev, void* buf, size_t buf_size,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	return i2c_ioop(((i2c_bus_data*) bus->bus_data)->fh,
			((i2c_dev_data*) dev->dev_bus_data)->address, buf, buf_size, read,
			cb_error, buf_msg, buf_size_msg);
}

bus_drv* get_i2c_drv()
{
	return &handle;
}

static int i2c_ioop(int fh, uint8_t address, void* buf, size_t buf_size,
		ssize_t (*ioop)(int fd, void* buf, size_t count),
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	ssize_t result = -1;
	char error_msg[BUF_LEN];

	/* set slave address */
	if (ioctl(fh, I2C_SLAVE, address) < 0)
	{
		snprintf(error_msg, BUF_LEN, "ioctl error: %s", strerror(errno));
		if (cb_error != NULL)
		{
			cb_error(error_msg, buf_msg, buf_size_msg);
		}
		return -1;
	}

	/* read/write from/to I2C */
	result = ioop(fh, buf, buf_size);
	if (result < 0 && cb_error != NULL)
	{
		snprintf(error_msg, BUF_LEN, "Error performing IO with file: %s",
				strerror(errno));
		cb_error(error_msg, buf_msg, buf_size_msg);
	}
	return result;
}

static ssize_t write_unconst(int fd, void *buf, size_t count)
{
	return write(fd, (const void*) buf, count);
}

static int i2c_open(io_bus* bus)
{
	i2c_bus_data* data = bus->bus_data;
	TRACE(printf("Opening I2C bus: %s\n", data->device));
	data->fh = open(data->device, O_RDWR);
	if (data->fh < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
static int i2c_close(io_bus* bus)
{
	i2c_bus_data* data = bus->bus_data;
	TRACE(printf("Closing I2C bus: %s\n", data->device));
	if (close(data->fh) < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static void* get_i2c_bus_data(const char* parameter)
{
	i2c_bus_data* data = malloc(sizeof(i2c_bus_data));
	if (NULL != data)
	{
		strncpy(data->device, parameter, I2C_DEV_LEN);
		data->fh = -1;
	}
	return data;
}

static void* get_i2c_dev_data(const char* parameter)
{
	i2c_dev_data* data = malloc(sizeof(i2c_dev_data));
	if (NULL != data)
	{
		data->address = strtol(parameter, NULL, 16);
	}
	return data;
}

static void discard_i2c_bus_data(void* bus_data)
{
	free(bus_data);
}

static void discard_i2c_bus_dev_data(void* bus_dev_data)
{
	free(bus_dev_data);
}
