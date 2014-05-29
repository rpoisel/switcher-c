/*
 * piface.c
 *
 *  Created on: May 27, 2014
 *      Author: rpoisel
 */

#include <stdlib.h>

#include "io.h"

#include "pfio.h"
#include "piface.h"

static int piface_write(int fh, uint8_t address, const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static uint32_t piface_init(void* init_value);
static int piface_read(int fh, uint8_t address, uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);

static io_drv handle =
{ .init = &piface_init, .read = &piface_read, .write = &piface_write };

io_drv* get_piface_drv()
{
	return &handle; /* not implemented yet */
}

static int piface_write(int fh, uint8_t address, const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	return 0;
}

static uint32_t piface_init(void* init_value)
{
	return 0;
}

static int piface_read(int fh, uint8_t address, uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	return 0;
}
