#include <stdlib.h>

#include "io.h"

int perform_io(io_config* config, io_data* data, io_cmd cmd,
		int (*cb_success)(io_data* data, char* buf, int buf_size),
		int (*cb_error)(char* error_msg, char* buf, int buf_size), char* buf,
		int buf_size)
{
	if (data->idx_bus < config->num_busses)
	{
		if (data->idx_dev < (config->busses + data->idx_bus)->num_devices)
		{
			if (cmd == CMD_WRITE)
			{
				if (((config->busses + data->idx_bus)->devices + data->idx_dev)->drv_handle->write(
						(config->busses + data->idx_bus)->fh,
						((config->busses + data->idx_bus)->devices
								+ data->idx_dev)->address, &data->value,
						cb_error, buf, buf_size) > 0)
				{
					cb_success(data, buf, buf_size);
				}
				/* error case has to be handled by write() function call */
			}
			else if (cmd == CMD_READ)
			{
				if (((config->busses + data->idx_bus)->devices + data->idx_dev)->drv_handle->read(
						(config->busses + data->idx_bus)->fh,
						((config->busses + data->idx_bus)->devices
								+ data->idx_dev)->address, &data->value,
						cb_error, buf, buf_size) > 0)
				{
					cb_success(data, buf, buf_size);
				}
			}
			else
			{
				cb_error("Illegal command.", buf, buf_size);
			}
		}
		else
		{
			cb_error("Device ID not existing.", buf, buf_size);
		}
	}
	else
	{
		cb_error("Bus ID not existing.", buf, buf_size);
	}

	return EXIT_SUCCESS;
}
