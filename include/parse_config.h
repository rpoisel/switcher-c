#ifndef PARSE_H_
#define PARSE_H_ 1

#include "io_fwd.h"

int parse_config(const char* filename, io_config* config);
int validate_config(io_config* bus_config);

#endif /* PARSE_H_ */
