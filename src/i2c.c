#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "i2c_io.h"

int i2c_init_fhs(i2c_config* config)
{
    unsigned cnt = 0;

    for (cnt = 0; cnt < config->num_busses; cnt++)
    {
        i2c_bus* current_bus = config->busses + cnt;
        current_bus->fh = open(current_bus->dev_file, O_RDWR);
        if (current_bus->fh == -1)
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int i2c_close_fhs(i2c_config* config)
{
    unsigned cnt = 0;

    for (cnt = 0; cnt < config->num_busses; cnt++)
    {
        i2c_bus* current_bus = config->busses + cnt;
        if (close(current_bus->fh) == -1)
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
