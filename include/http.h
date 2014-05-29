#ifndef HTTP_H_
#define HTTP_H_ 1

/* 3rd party libraries */
#include "mongoose.h"

/* own includes */
#include "io_fwd.h"

#define MAX_NUM_CONF 128
#define STR_CONF_LEN 256

struct mg_context* start_http_server(const char *http_options[],
		io_config* us_config);

int stop_http_server(struct mg_context* context);

int io_data_to_json(io_data* data, char* buf, int buf_size);
int error_to_json(char* error_msg, char* buf, int buf_size);

#endif /* HTTP_H_ */
