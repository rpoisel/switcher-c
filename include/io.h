#ifndef IO_H_
#define IO_H _1

#include <stdint.h>

#include <io_fwd.h>

#define MAX_BUSSES 8
#define MAX_IO_DEVICES 128

/* e.g. /i2c-2 */
#define MAX_PARAM_LEN 32

typedef enum io_cmd
{
	CMD_NONE, CMD_WRITE, CMD_READ,
} io_cmd;

typedef enum bus_type
{
	BUS_NONE, BUS_I2C, BUS_PIFACE
} bus_type;

typedef struct dev_drv
{
	uint32_t (*init)(io_bus* bus, io_dev* dev);
	int (*read)(io_bus* bus, io_dev* dev, value_t* value,
			int (*cb_error)(char* error_msg, char* buf, int buf_size),
			char* buf_msg, int buf_size_msg);
	int (*write)(io_bus* bus, io_dev* dev, const value_t* value,
			int (*cb_error)(char* error_msg, char* buf, int buf_size),
			char* buf_msg, int buf_size_msg);
	uint32_t (*deinit)(io_bus* bus, io_dev* dev);
} dev_drv;

typedef struct io_dev
{
	dev_drv* drv_handle;
	void* dev_bus_data; /* device specific for bus */
	void* dev_data; /* device specific */
} io_dev;

typedef struct bus_drv_
{
	void* (*get_bus_data)(const char* parameter);
	void* (*get_bus_dev_data)(const char* parameter);
	void (*discard_bus_data)(void* bus_data);
	void (*discard_bus_dev_data)(void* bus_dev_data);
	int (*open)(io_bus* bus);
	int (*close)(io_bus* bus);
} bus_drv;

typedef int32_t idx_t;
#define IDX_INVALID -1

typedef struct io_bus
{
	idx_t num_devices;
	io_dev devices[MAX_IO_DEVICES];
	bus_drv* drv_handle;
	void* bus_data; /* bus specific */
} io_bus;

typedef struct io_config
{
	idx_t num_busses;
	io_bus busses[MAX_BUSSES];
} io_config;

typedef struct io_data
{
	idx_t idx_bus;
	idx_t idx_dev;
	idx_t idx_sub_dev;
	value_t value;
} io_data;

#define INIT_IO_DATA(p) \
	p->idx_bus = IDX_INVALID; \
	p->idx_dev = IDX_INVALID; \
	p->idx_sub_dev = IDX_INVALID; \
	p->value = 0;

int perform_io(io_config* config, io_data* data, io_cmd cmd,
		int (*cb_success)(io_data* data, char* buf, int buf_size),
		int (*cb_error)(char* error_msg, char* buf, int buf_size), char* buf,
		int buf_size);

int conf_init(io_config* config);
int conf_deinit(io_config* config);
int io_init(io_config* config);
int io_deinit(io_config* config);

#endif /* IO_H_ */
