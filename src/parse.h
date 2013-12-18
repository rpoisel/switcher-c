#ifndef __PARSE_H__
#define __PARSE_H__ 1

#include "i2c_io.h"

int parse_config(const char* pFilename, i2c_config* pConfig);
int validate_config(i2c_config* i2c_bus_config);

#endif
