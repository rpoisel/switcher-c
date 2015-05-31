/*
 * dev_piface.c
 *
 *  Created on: May 27, 2014
 *      Author: rpoisel
 */

#include <dev_piface.h>
#include <stdlib.h>

#include <trace.h>
#include <io.h>
#include <pfio.h>

static uint32_t piface_init(io_bus* bus, io_dev* dev);
static int piface_write(io_bus* bus, io_dev* dev,
		const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static int piface_read(io_bus* bus, io_dev* dev,
		value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg);
static uint32_t piface_deinit(io_bus* bus, io_dev* dev);

static dev_drv handle =
{ .init = &piface_init, .read = &piface_read, .write = &piface_write, .deinit =
		&piface_deinit };

dev_drv* get_piface_drv()
{
	return &handle; /* not implemented yet */
}

static int piface_write(io_bus* bus /* ignored */, io_dev* dev /* ignored */,
		const value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	pfio_write_output(*value);
	return 1;
}

static uint32_t piface_init(io_bus* bus, io_dev* dev)
{
	TRACE(printf("Initializing PiFace device.\n"));
	if (pfio_init() < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static int piface_read(io_bus* bus /* ignored */, io_dev* dev /* ignored */,
		value_t* value,
		int (*cb_error)(char* error_msg, char* buf, int buf_size),
		char* buf_msg, int buf_size_msg)
{
	(*value) = pfio_read_output();
	return 1;
}

static uint32_t piface_deinit(io_bus* bus, io_dev* dev)
{
	TRACE(printf("Deinitializing PiFace device.\n"));
	if (pfio_deinit() < 0)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
