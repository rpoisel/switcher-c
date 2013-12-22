#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ini.h"

#include "parse_i2c.h"
#include "i2c_io.h"
#include "pcf8574.h"

#define MAX_INI_ENTRY_LEN 128
#define INI_BUS "bus"
#define INI_DEV "device"
#define INI_DEV_DRV "devdrv"
#define INI_DEV_FILE "devicefile"
#define INI_DEV_ADR "address"
#define INI_DRV_PCF8574 "pcf8574"

typedef struct
{
    i2c_config *config;
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

static int process_entries(i2c_bus* current_bus, i2c_io* current_io, const char* name, const char* value)
{
    if (strncmp(name, INI_DEV_FILE, MAX_INI_ENTRY_LEN) == 0)
    {
        strncpy(current_bus->dev_file, value, MAX_DEV_FILE_LEN);
    }
    else if (strncmp(name, INI_DEV_DRV, MAX_INI_ENTRY_LEN) == 0)
    {
        if (strncmp(value, INI_DRV_PCF8574, MAX_INI_ENTRY_LEN) == 0)
        {
            current_io->drv_handle = get_pcf8574_drv();
        }
    }
    else if (strncmp(name, INI_DEV_ADR, MAX_INI_ENTRY_LEN) == 0)
    {
        current_io->address = strtol(value, NULL, 16);
    }
    else
    {
        return 0; /* unknown entry */
    }

    return 1; /* success (as defined by inih) */
}

/**
 * invoked by inih
 */
static int parser_handler(void* user, const char* section, const char* name,
        const char* value)
{
    bus_configs* user_data = (bus_configs*)user;

    update_user_data(user_data, section);

    i2c_bus* current_bus = user_data->config->busses + user_data->current_bus_id;
    i2c_io* current_io = current_bus->devices + user_data->current_dev_id;

    current_bus->num_devices = user_data->current_dev_id + 1;
    current_bus->fh = -1;

#if 0
    printf("Section: %s, Name: %s, Value: %s\n", section, name, value);
#endif

    return process_entries(current_bus, current_io, name, value);
}

int validate_config(i2c_config* i2c_bus_config)
{
    return EXIT_FAILURE; /* error (not implemented)*/
}

int parse_config(const char* filename, i2c_config* config)
{
    bus_configs user_data = {
        .last_section[0] = '\0',
        .config = config,
        .current_bus_id = -1,
        .current_dev_id = -1
    };
    if (ini_parse(filename, &parser_handler, &user_data) < 0) {
        fprintf(stderr, "Can't load '%s'\n", filename);
        return EXIT_FAILURE;
    }

    return 0;
}
