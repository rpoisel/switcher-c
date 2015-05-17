#ifndef MCP23017_H_
#define MCP23017_H_ 1

#include "io_fwd.h"

typedef struct mcp23017_data_
{
	uint8_t mode; /* read or write */
} mcp23017_data;


dev_drv* get_mcp23017_drv();
void get_mcp23017_data(mcp23017_data* data);

#endif /* MCP23017_H_ */
