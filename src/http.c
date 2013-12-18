/* standard includes */
#include <string.h>

/* 3rd party libraries */

/* own includes */
#include "http.h"
#include "i2c_io.h"

#define STRTOK_BUF_SIZE 1024
#define URI_DELIM "/"

typedef enum
{
    REQUEST_NONE,
    REQUEST_GET,
    REQUEST_SET,
} request_type;

typedef struct
{
    unsigned idx_bus;
    unsigned idx_dev;
    unsigned value;
} uri_parts;

static int begin_request_handler(struct mg_connection *conn);

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

    return 0; /* success */
}

static request_type parse_uri(const char* uri, uri_parts* parts)
{
    request_type result = REQUEST_NONE;

    char buf_uri[STRTOK_BUF_SIZE];

    char* rest;
    char* token = NULL;
    unsigned cnt = 0;

    /* tbd create copy of uri */

    strncpy(buf_uri, uri, STRTOK_BUF_SIZE);
    token = strtok_r(buf_uri, URI_DELIM, &rest);
    while (cnt < 3 && token != NULL)
    {
        printf("URI part: %s\n", token);
        token = strtok_r(NULL, URI_DELIM, &rest);

        switch(cnt)
        {
            case 0:
                /* bus index */
                break;
            case 1:
                /* device index */
                result = REQUEST_GET;
                break;
            case 2:
                /* value */
                result = REQUEST_SET;
                break;
            default:
                /* illegal value */
                result = REQUEST_NONE;
                break;
        }
        ++cnt;
    }

    return result;
}

// This function will be called by mongoose on every new request.
static int begin_request_handler(struct mg_connection *conn) 
{
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    char content[1024];

    uri_parts parts;

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



    parse_uri(request_info->uri, &parts);
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

