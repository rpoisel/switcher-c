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

static uint32_t piface_init(void* init_value);
static int piface_write(int fh, uint8_t address, const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int piface_read(int fh, uint8_t address, uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static uint32_t piface_deinit(void* deinit_value);

static io_drv handle =
{ .init = &piface_init, .read = &piface_read, .write = &piface_write, .deinit =
		&piface_deinit };

io_drv* get_piface_drv()
{
	return &handle; /* not implemented yet */
}

static int piface_write(int fh /* ignored */, uint8_t address /* ignored */,
		const uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	/* pfio_digital_write() */
	uint8_t cnt = 0;

	for (cnt = 0; cnt < 8; cnt++)
	{
		pfio_digital_write(cnt, ((*value) & (1 << cnt)) == (1 << cnt));
	}
	return 1;
}

static uint32_t piface_init(void* init_value)
{
	if (pfio_init() < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static int piface_read(int fh /* ignored */, uint8_t address /* ignored */,
		uint32_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	uint8_t cnt = 0;
	uint8_t pin_value = 0;

	for (cnt = 0; cnt < 8; cnt++)
	{
		pin_value = pfio_digital_read(cnt);
		(*value) |= (pin_value << cnt);
	}
	return 1;
}

static uint32_t piface_deinit(void* deinit_value)
{
	if (pfio_deinit() < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
