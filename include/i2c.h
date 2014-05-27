#ifndef __I2C_H__
#define __I2C_H__ 1

#include <stdint.h>

#include "i2c_io_fwd.h"

int i2c_init_fhs(i2c_config* config);
int i2c_close_fhs(i2c_config* config);

int i2c_write(int fh, uint8_t address, void* buf, size_t buf_size,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);

int i2c_read(int fh, uint8_t address, void* buf, size_t buf_size,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf_msg,
        int buf_size_msg);

ssize_t write_unconst(int fd, void *buf, size_t count);

#endif /* __I2C_H__ */
