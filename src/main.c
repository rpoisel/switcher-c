/* standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>

/* included libraries */

/* own functions */
#include "parse_i2c.h"
#include "http.h"
#include "i2c.h"
#include "i2c_io.h"

#define DEFAULT_CONFIG_PATH "../config/io_ext.ini"
#define DAEMON_NAME "switcher"

static struct mg_context* http_context = NULL;
static i2c_config i2c_bus_config;
static char *http_options[MAX_NUM_CONF] = { NULL };

static void signal_handler(int sig);

/*
 * Useful hints:
 *   * http://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
 */

int main(int argc, char* argv[]) 
{
    int option = -1;
    char config_path[STR_CONF_LEN];
    int cnt_option = 0;

#define ADD_OPTION_ELEMENT(option) \
    http_options[cnt_option] = (char*)malloc(STR_CONF_LEN); \
    strncpy(http_options[cnt_option], option, STR_CONF_LEN); \
    cnt_option++;

    ADD_OPTION_ELEMENT("listening_ports")
    ADD_OPTION_ELEMENT("8080")

    openlog(DAEMON_NAME, LOG_PID, LOG_DAEMON);

    if (signal(SIGINT, signal_handler) == SIG_ERR || 
            signal(SIGTERM, signal_handler) == SIG_ERR)
    {
        syslog(LOG_ERR, "Could not install signal handler for SIGINT.");
        return 1;
    }

    strncpy(config_path, DEFAULT_CONFIG_PATH, STR_CONF_LEN);
    while ((option = getopt(argc, argv, "hc:p:")) != -1)
    {
        switch (option)
        {
            case 'c':
                strncpy(config_path, optarg, STR_CONF_LEN);
                break;
            case 'p':
                strncpy(http_options[1], optarg, STR_CONF_LEN);
                break;
            default:
                fprintf(stderr, "Usage: %s [-h] [-c config-file] [-p port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (parse_config(config_path, &i2c_bus_config) == EXIT_FAILURE)
    {
        syslog(LOG_ERR, "Error parsing the configuration file.");
        exit(EXIT_FAILURE);
    }
    validate_config(&i2c_bus_config);

    if (i2c_init_fhs(&i2c_bus_config) == EXIT_FAILURE)
    {
        syslog(LOG_ERR, "Could not open I2C device files.");
        exit(EXIT_FAILURE);
    }

    http_context = start_http_server((const char **)http_options, &i2c_bus_config);

    while (1)
    {
        getchar();
    }

    return 0;
}

static void signal_handler(int sig)
{
    int ret = 0;
    int cnt = 0;

    if (NULL != http_context)
    {
        syslog(LOG_INFO, "Shutting down HTTP server.");
        stop_http_server(http_context);
    }

    syslog(LOG_INFO, "Closing I2C filehandles.");
    i2c_close_fhs(&i2c_bus_config);

    for (cnt = 0; cnt < MAX_NUM_CONF; cnt++)
    {
        if (http_options[cnt] != NULL)
        {
            free(http_options[cnt]);
        }
    }

    closelog();

    exit(ret);
}
