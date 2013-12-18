#ifndef __HTTP_H__
#define __HTTP_H__ 1

/* 3rd party libraries */
#include "mongoose.h"

/* own includes */
#include "i2c_io.h"

struct mg_context* start_http_server(const char *http_options[],
        i2c_config* i2c_bus_config);

int stop_http_server(struct mg_context* context);

#endif