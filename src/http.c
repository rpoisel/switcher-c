/* standard includes */
#include <string.h>

/* 3rd party libraries */

/* own includes */
#include "http.h"
#include "i2c_io.h"

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

// This function will be called by mongoose on every new request.
static int begin_request_handler(struct mg_connection *conn) 
{
  const struct mg_request_info *request_info = mg_get_request_info(conn);
  char content[1024];

#if 0
  char* rest;
  char* token = NULL;
#endif

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



#if 0
  token = strtok_r(request_info->uri, "/", &rest);
  while (token != NULL)
  {
      printf("URI part: %s\n", token);
      token = strtok_r(NULL, "/", &rest);
  }
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

