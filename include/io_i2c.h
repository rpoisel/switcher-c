#ifndef IO_I2C_H_
#define IO_I2C_H_ 1

#include <stdint.h>

#include "io_fwd.h"

int i2c_write(int fh, uint8_t address, void* buf, size_t buf_size,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

int i2c_read(int fh, uint8_t address, void* buf, size_t buf_size,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

ssize_t write_unconst(int fd, void *buf, size_t count);

#endif /* IO_I2C_H_ */
