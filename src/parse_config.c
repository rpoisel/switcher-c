#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "ini.h"

#include "parse_config.h"
#include "io.h"
#include "pcf8574.h"
#include "piface.h"

#define MAX_INI_ENTRY_LEN 128
#define INI_BUS "bus"
#define INI_BUS_TYPE "bus_type"
#define INI_BUS_TYPE_I2C "i2c"
#define INI_BUS_TYPE_PIFACE "piface"
#define INI_DEV "device"
#define INI_DEV_DRV "devdrv"
#define INI_BUS_PARAM_0 "bus_parameter_0"
#define INI_DEV_ADR "address"
#define INI_DRV_PCF8574 "pcf8574"
#define INI_DRV_PIFACE "piface"

typedef struct
{
	io_config *config;
	char last_section[MAX_INI_ENTRY_LEN];
	int current_bus_id;
	int current_dev_id;
} bus_configs;

static void update_user_data(bus_configs* user_data, const char* section)
{
	/* determine current indices */
	if (strncmp(user_data->last_section, section, MAX_INI_ENTRY_LEN))
	{
		if (strncmp(section, INI_BUS, strlen(INI_BUS)) == 0)
		{
			++user_data->current_bus_id;
			user_data->current_dev_id = -1;
		}
		else if (strncmp(section, INI_DEV, strlen(INI_DEV)) == 0)
		{
			++user_data->current_dev_id;
		}
	}
	strncpy(user_data->last_section, section, MAX_INI_ENTRY_LEN);
	user_data->config->num_busses = user_data->current_bus_id + 1;
}

static int process_entries(io_bus* current_bus, io_dev* current_dev,
		const char* name, const char* value)
{
	if (strncmp(name, INI_BUS_PARAM_0, MAX_INI_ENTRY_LEN) == 0)
	{
		strncpy(current_bus->param_0, value, MAX_PARAM_LEN);
	}
	else if (strncmp(name, INI_BUS_TYPE, MAX_INI_ENTRY_LEN) == 0)
	{
		if (strncmp(value, INI_BUS_TYPE_PIFACE, MAX_PARAM_LEN) == 0)
		{
			current_bus->type = BUS_PIFACE;
		}
		else
		{
			current_bus->type = BUS_I2C;
		}
	}
	else if (strncmp(name, INI_DEV_DRV, MAX_INI_ENTRY_LEN) == 0)
	{
		if (strncmp(value, INI_DRV_PCF8574, MAX_INI_ENTRY_LEN) == 0)
		{
			current_dev->drv_handle = get_pcf8574_drv();
		}
		else if (strncmp(value, INI_DRV_PIFACE, MAX_INI_ENTRY_LEN) == 0)
		{
			current_dev->drv_handle = get_piface_drv();
		}
		current_bus->num_devices += 1;
	}
	else if (strncmp(name, INI_DEV_ADR, MAX_INI_ENTRY_LEN) == 0)
	{
		current_dev->address = strtol(value, NULL, 16);
	}
	else
	{
		return 0; /* unknown entry */
	}

	return 1; /* success (as defined by ini.h) */
}

/**
 * invoked by inih
 */
static int parser_handler(void* user, const char* section, const char* name,
		const char* value)
{
	bus_configs* user_data = (bus_configs*) user;

	update_user_data(user_data, section);

	io_bus* current_bus = user_data->config->busses + user_data->current_bus_id;
	io_dev* current_dev = current_bus->devices + user_data->current_dev_id;

#if 0
	printf("Section: %s, Name: %s, Value: %s\n", section, name, value);
#endif

	return process_entries(current_bus, current_dev, name, value);
}

int validate_config(io_config* bus_config)
{
	return EXIT_SUCCESS; /* not implemented yet */
}

int parse_config(const char* filename, io_config* config)
{
	bus_configs user_data =
	{ .last_section[0] = '\0', .config = config, .current_bus_id = -1,
			.current_dev_id = -1 };
	if (ini_parse(filename, &parser_handler, &user_data) < 0)
	{
		syslog(LOG_ERR, "Can't load '%s'\n", filename);
		return EXIT_FAILURE;
	}

	return 0;
}
