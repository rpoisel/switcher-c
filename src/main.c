/* standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/* included libraries */

/* own functions */
#include "parse.h"
#include "http.h"
#include "i2c.h"
#include "i2c_io.h"

#define STR_PATH_LEN 256
#define DEFAULT_CONFIG_PATH "../config/io_ext.ini"

static struct mg_context* http_context = NULL;
static i2c_config i2c_bus_config;

static void signal_handler(int sig);

int main(int argc, char* argv[]) 
{
    int option = -1;
    char config_path[STR_PATH_LEN];
    const char *http_options[] = {"listening_ports", "8080", NULL};

    if (signal(SIGINT, signal_handler) == SIG_ERR || 
            signal(SIGTERM, signal_handler) == SIG_ERR)
    {
        fprintf(stderr, "Could not install signal handler for SIGINT.\n");
        return 1;
    }

    strncpy(config_path, DEFAULT_CONFIG_PATH, STR_PATH_LEN);
    while ((option = getopt(argc, argv, "hc:")) != -1)
    {
        switch (option)
        {
            case 'c':
                strncpy(config_path, optarg, STR_PATH_LEN);
                break;
            default:
                fprintf(stderr, "Usage: %s [-h] [-c config-file]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (parse_config(config_path, &i2c_bus_config) == EXIT_FAILURE)
    {
        exit(EXIT_FAILURE);
    }
    validate_config(&i2c_bus_config);

#if 0
    /* test parsing functionality */
    i2c_bus_config.busses[0].devices[0].drv_handle->write(0x38, 0xFF);
    printf("Address: 0x%02X\n", i2c_bus_config.busses[0].devices[0].address);
    printf("Number of busses: %d\n", i2c_bus_config.num_busses);
#endif

    i2c_init_fhs(&i2c_bus_config);

    http_context = start_http_server(http_options, &i2c_bus_config);

    while (1)
    {
        getchar();
    }

    return 0;
}

static void signal_handler(int sig)
{
    int ret = 0;

    if (NULL != http_context)
    {
        fprintf(stderr, "Shutting down HTTP server ... ");
        fflush(stderr);
        stop_http_server(http_context);
        fprintf(stderr, "done.\n");
    }

    fprintf(stderr, "Closing I2C filehandles ... ");
    fflush(stderr);
    i2c_close_fhs(&i2c_bus_config);
    fprintf(stderr, "done.\n");

    exit(ret);
}
