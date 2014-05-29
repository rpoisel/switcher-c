#ifndef IO_H_
#define IO_H _1

#include <stdint.h>

#define MAX_BUSSES 8
#define MAX_IO_DEVICES 128

/* f.g. /i2c-2 */
#define MAX_PARAM_LEN 32

typedef enum io_cmd
{
	CMD_NONE, CMD_WRITE, CMD_READ,
} io_cmd;

typedef enum bus_type
{
	BUS_NONE, BUS_I2C, BUS_PIFACE
} bus_type;

typedef struct io_drv
{
	/* init device */
	uint32_t (*init)(void* init_value);

	/* read IOs */
	int (*read)(int fh, uint8_t address, uint32_t* value,
			int (*cb_error)(char* error_msg, char* buf, int buf_size),
			char* buf_msg, int buf_size_msg);

	/* write IOs */
	int (*write)(int fh, uint8_t address, const uint32_t* value,
			int (*cb_error)(char* error_msg, char* buf, int buf_size),
			char* buf_msg, int buf_size_msg);

	/* deinit device */
	uint32_t (*deinit)(void* deinit_value);
} io_drv;

typedef struct io_dev
{
	uint8_t address;
	io_drv* drv_handle;
} io_dev;

typedef struct io_bus
{
	bus_type type;
	unsigned num_devices;
	io_dev devices[MAX_IO_DEVICES];
	char param_0[MAX_PARAM_LEN];
	int fh;
} io_bus;

typedef struct io_config
{
	unsigned num_busses;
	io_bus busses[MAX_BUSSES];
} io_config;

typedef struct io_data
{
	unsigned idx_bus;
	unsigned idx_dev;
	uint32_t value;
} io_data;

int perform_io(io_config* config, io_data* data, io_cmd cmd,
		int (*cb_success)(io_data* data, char* buf, int buf_size),
		int (*cb_error)(char* error_msg, char* buf, int buf_size), char* buf,
		int buf_size);

int conf_init(io_config* config);
int io_init(io_config* config);
int io_deinit(io_config* config);

#endif /* IO_H_ */
