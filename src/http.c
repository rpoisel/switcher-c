/* standard includes */
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/* 3rd party libraries */

/* own includes */
#include "http.h"
#include "i2c_io.h"

#define STRTOK_BUF_SIZE 1024
#define URI_DELIM "/"

typedef enum
{
    REQUEST_ERR,
    REQUEST_GET,
    REQUEST_SET,
} request_type;

typedef struct
{
    unsigned idx_bus;
    unsigned idx_dev;
    unsigned value;
} uri_parts;

typedef enum
{
    STAGE_BUS = 0,
    STAGE_DEV,
    STAGE_VALUE
} uri_stages;

static int begin_request_handler(struct mg_connection *conn);

static void print_debug_request(const char* uri, uri_parts* parts, request_type type);

struct mg_context* start_http_server(const char *http_options[], i2c_config* i2c_bus_config)
{
    struct mg_callbacks callbacks;

    // Prepare callbacks structure. We have only one callback, the rest are NULL.
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = begin_request_handler;

    return mg_start(&callbacks, NULL, http_options);

}

int stop_http_server(struct mg_context* context)
{
    // Stop the server.
    mg_stop(context);

    return EXIT_SUCCESS;
}

static request_type parse_uri(const char* uri, uri_parts* parts)
{
    request_type result = REQUEST_ERR;

    char buf_uri[STRTOK_BUF_SIZE];

    char* rest;
    char* token = NULL;
    unsigned cnt_stage = 0;
    long number = 0;
    char* endptr = NULL;
    int base = 10;

    /* circumvent const => unconst cast */
    strncpy(buf_uri, uri, STRTOK_BUF_SIZE);

    token = strtok_r(buf_uri, URI_DELIM, &rest);
    while (cnt_stage <= STAGE_VALUE && token != NULL)
    {
        base = (cnt_stage == STAGE_VALUE ? 16 : 10);

        endptr = NULL;
        number = strtol(token, &endptr, base);

        if ((errno == ERANGE && (number == LONG_MAX || number == LONG_MIN))
                || endptr == token)
        {
            return REQUEST_ERR;
        }

        /* assign parsed number to current stage's counterpart in data structure */
        switch (cnt_stage)
        {
            case STAGE_BUS:
                parts->idx_bus = number;
                break;
            case STAGE_DEV:
                parts->idx_dev = number;
                result = REQUEST_GET;
                break;
            case STAGE_VALUE:
                parts->value = number;
                result = REQUEST_SET;
                break;
            default:
                /* illegal stage */
                result = REQUEST_ERR;
                break;
        }
        ++cnt_stage;
        token = strtok_r(NULL, URI_DELIM, &rest);
    }

    return result;
}

// This function will be called by mongoose on every new request.
static int begin_request_handler(struct mg_connection *conn) 
{
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char content[1024];

    uri_parts parts;
    request_type type = REQUEST_ERR;

    // Prepare the message we're going to send
    int content_length = snprintf(content, sizeof(content),
            "{"
            "\"msg\": \"%s\","
            "\"remotePort\": %d,"
            "\"queryString\": \"%s\","
            "\"uri\": \"%s\""
            "}",
            "Hello from mongoose!",
            request_info->remote_port,
            request_info->query_string,
            request_info->uri);

    type = parse_uri(request_info->uri, &parts);

#if 1
    print_debug_request(request_info->uri, &parts, type);
#endif

    // Send HTTP reply to the client
    mg_printf(conn,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n"        // Always set Content-Length
            "\r\n"
            "%s",
            content_length, content);

    // Returning non-zero tells mongoose that our function has replied to
    // the client, and mongoose should not send client any more data.
    return 1;
}


static void print_debug_request(const char* uri, uri_parts* parts, request_type type)
{
    fprintf(stderr, "URI: %s, ", uri);
    switch (type)
    {
        case REQUEST_GET:
            fprintf(stderr, "Request: GET, ");
            break;
        case REQUEST_SET:
            fprintf(stderr, "Request: SET, ");
            break;
        default:
            fprintf(stderr, "Request: ERR");
            break;
    }
    if (REQUEST_ERR != type)
    {
        fprintf(stderr, "Bus: %d, Device: %d", parts->idx_bus, parts->idx_dev);
        if (REQUEST_SET == type)
        {
            fprintf(stderr, ", Value: 0x%02X", parts->value);
        }
    }
    fprintf(stderr, "\n");
}
