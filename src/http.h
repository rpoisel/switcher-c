#ifndef __HTTP_H__
#define __HTTP_H__ 1

/* 3rd party libraries */
#include "mongoose.h"

/* own includes */
#include "i2c_io.h"

#define MAX_NUM_CONF 128
#define STR_CONF_LEN 256

struct mg_context* start_http_server(const char *http_options[],
        i2c_config* i2c_bus_config);

int stop_http_server(struct mg_context* context);

int i2c_data_to_json(i2c_data* data, char* buf, int buf_size);
int error_to_json(char* error_msg, char* buf, int buf_size);

#endif
