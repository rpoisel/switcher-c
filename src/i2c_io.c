#include <stdlib.h>

#include "i2c_io.h"

int perform_i2c_io(i2c_config* config, i2c_data* data, i2c_cmd cmd,
        int (*cb_success)(i2c_data* data, char* buf, int buf_size),
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf,
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
                    ((config->busses + data->idx_bus)->devices + data->idx_dev)->address,
                    &data->value,
                    cb_error,
                    buf,
                    buf_size
                    ) > 0)
                {
                    cb_success(data, buf, buf_size);
                }
                /* error case has to be handled by write() function call */
            }
            else if (cmd == CMD_READ)
            {
                cb_success(data, buf, buf_size);
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
