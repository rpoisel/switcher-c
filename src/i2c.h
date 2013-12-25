#ifndef __I2C_H__
#define __I2C_H__ 1

#include "i2c.h"
#include "i2c_io.h"

int i2c_init_fhs(i2c_config* config);
int i2c_close_fhs(i2c_config* config);

int i2c_write(int fh, uint8_t address, uint32_t value,
        int (*cb_error)(char* error_msg, char* buf, int buf_size),
        char* buf,
        int buf_size);

#endif
