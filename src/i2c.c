#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <linux/i2c-dev.h>

#include "i2c.h"
#include "i2c_io.h"

#define BUF_LEN 256

static int i2c_ioop(int fh, uint8_t address, void* buf, size_t buf_size,
        ssize_t (*ioop)(int fd, void* buf, size_t count),
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);

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

int i2c_write(int fh, uint8_t address, void* buf, size_t buf_size,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg)
{
    return i2c_ioop(fh, address, buf, buf_size, write_unconst, cb_error, buf_msg, buf_size_msg);
}

int i2c_read(int fh, uint8_t address, void* buf, size_t buf_size,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg)
{
    return i2c_ioop(fh, address, buf, buf_size, read, cb_error, buf_msg, buf_size_msg);
}

static int i2c_ioop(int fh, uint8_t address, void* buf, size_t buf_size,
        ssize_t (*ioop)(int fd, void* buf, size_t count),
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg)
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
        snprintf(error_msg, BUF_LEN, "Error writing file: %s", strerror(errno));
        cb_error(error_msg, buf_msg, buf_size_msg);
    }
    return result;
}

ssize_t write_unconst(int fd, void *buf, size_t count)
{
    return write(fd, (const void*)buf, count);
}
