#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "io.h"

int conf_init(io_config* config)
{
	unsigned cnt_bus = 0;
	unsigned cnt_dev = 0;

	config->num_busses = 0;
	for (cnt_bus = 0; cnt_bus < MAX_BUSSES; cnt_bus++)
	{
		io_bus* current_bus = config->busses + cnt_bus;
		current_bus->num_devices = 0;
		for (cnt_dev = 0; cnt_dev < MAX_IO_DEVICES; cnt_dev++)
		{
			io_dev* current_dev = current_bus->devices + cnt_dev;
			current_dev->dev_bus_data = NULL;
			current_dev->dev_data = NULL;
			current_dev->drv_handle = NULL;
		}
		current_bus->bus_data = NULL;
	}
	return EXIT_SUCCESS;
}

int conf_deinit(io_config* config)
{
	unsigned cnt_bus = 0;
	unsigned cnt_dev = 0;

	config->num_busses = 0;
	for (cnt_bus = 0; cnt_bus < MAX_BUSSES; cnt_bus++)
	{
		io_bus* current_bus = (config->busses + cnt_bus);
		for (cnt_dev = 0; cnt_dev < MAX_IO_DEVICES; cnt_dev++)
		{
			io_dev* current_dev = current_bus->devices + cnt_dev;
			if (NULL != current_bus->drv_handle
					&& NULL != current_bus->drv_handle->discard_bus_dev_data)
			{
				current_bus->drv_handle->discard_bus_dev_data(
						current_dev->dev_bus_data);
				current_dev->dev_bus_data = NULL;
			}
		}
		if (NULL != current_bus->drv_handle
				&& NULL != current_bus->drv_handle->discard_bus_data)
		{
			current_bus->drv_handle->discard_bus_data(current_bus->bus_data);
			current_bus->bus_data = NULL;
		}
	}
	return EXIT_SUCCESS;
}

int io_init(io_config* config)
{
	idx_t cnt_bus = 0;
	idx_t cnt_dev = 0;

	for (cnt_bus = 0; cnt_bus < config->num_busses; cnt_bus++)
	{
		io_bus* current_bus = config->busses + cnt_bus;
		if (NULL != current_bus->drv_handle
				&& NULL != current_bus->drv_handle->open)
		{
			current_bus->drv_handle->open(current_bus);
		}
		for (cnt_dev = 0; cnt_dev < current_bus->num_devices; cnt_dev++)
		{
			io_dev* current_dev = current_bus->devices + cnt_dev;
			if (NULL != current_dev->drv_handle
					&& NULL != current_dev->drv_handle->init)
			{
				current_dev->drv_handle->init(current_bus, current_dev);
			}
		}
	}

	return EXIT_SUCCESS;
}

int io_deinit(io_config* config)
{
	idx_t cnt_bus = 0;
	idx_t cnt_dev = 0;

	for (cnt_bus = 0; cnt_bus < config->num_busses; cnt_bus++)
	{
		io_bus* current_bus = config->busses + cnt_bus;
		for (cnt_dev = 0; cnt_dev < current_bus->num_devices; cnt_dev++)
		{
			io_dev* current_dev = current_bus->devices + cnt_dev;
			if (NULL != current_dev->drv_handle
					&& NULL != current_dev->drv_handle->deinit)
			{
				current_dev->drv_handle->deinit(current_bus, current_dev);
			}
		}
		if (NULL != current_bus->drv_handle
				&& NULL != current_bus->drv_handle->close)
		{
			current_bus->drv_handle->close(current_bus);
		}
	}

	return EXIT_SUCCESS;
}

int perform_io(io_config* config, io_data* data, io_cmd cmd,
		int (*cb_success)(io_data* data, char* buf, int buf_size),
		int (*cb_error)(char* error_msg, char* buf, int buf_size), char* buf,
		int buf_size)
{
	int num_read = 0;
	value_t value = data->value;

	if (data->idx_bus == IDX_INVALID || data->idx_bus >= config->num_busses)
	{
		cb_error("Bus ID not existing.", buf, buf_size);
		return EXIT_FAILURE;
	}

	if (data->idx_dev == IDX_INVALID
			|| data->idx_dev >= (config->busses + data->idx_bus)->num_devices)
	{
		cb_error("Device ID not existing.", buf, buf_size);
		return EXIT_FAILURE;
	}

	if (cmd != CMD_WRITE && cmd != CMD_READ)
	{
		cb_error("Illegal command.", buf, buf_size);
		return EXIT_FAILURE;
	}

	if (cmd == CMD_WRITE)
	{
		if (data->idx_sub_dev != IDX_INVALID)
		{
			num_read = ((config->busses + data->idx_bus)->devices
					+ data->idx_dev)->drv_handle->read(
					(config->busses + data->idx_bus),
					((config->busses + data->idx_bus)->devices + data->idx_dev),
					&value, cb_error, buf, buf_size);
			if (num_read <= 0)
			{
				cb_error("Could not read from device.", buf, buf_size);
				return EXIT_FAILURE;
			}
			if (data->value)
			{
				value |= 1 << data->idx_sub_dev;
			}
			else
			{
				value &= ~(1 << data->idx_sub_dev);
			}
		}
		if (((config->busses + data->idx_bus)->devices + data->idx_dev)->drv_handle->write(
				(config->busses + data->idx_bus),
				((config->busses + data->idx_bus)->devices + data->idx_dev),
				&value, cb_error, buf, buf_size) > 0)
		{
			cb_success(data, buf, buf_size);
		}
	}
	else
	{
		if (((config->busses + data->idx_bus)->devices + data->idx_dev)->drv_handle->read(
				(config->busses + data->idx_bus),
				((config->busses + data->idx_bus)->devices + data->idx_dev),
				&data->value, cb_error, buf, buf_size) > 0)
		{
			if (data->idx_sub_dev != IDX_INVALID)
			{
				if (data->idx_sub_dev >= 0
						&& data->idx_sub_dev < (sizeof(data->value) * 8))
				{
					data->value = (data->value >> data->idx_sub_dev) & 1;
				}
				else
				{
					cb_error("Invalid sub device-index given.", buf, buf_size);
				}
			}
			cb_success(data, buf, buf_size);
		}
	}

	return EXIT_SUCCESS;
}
