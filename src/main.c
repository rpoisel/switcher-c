/* standard headers */
#include <stdio.h>
#include <string.h>

/* included libraries */
#include "mongoose.h"

/* own functions */
#include "parse.h"
#include "i2c.h"
#include "i2c_io.h"
#include "pcf8574.h"

// This function will be called by mongoose on every new request.
static int begin_request_handler(struct mg_connection *conn) {
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

int main(void) {
  struct mg_context *ctx;
  struct mg_callbacks callbacks;

  // List of options. Last element must be NULL.
  const char *options[] = {"listening_ports", "8080", NULL};

  i2c_config lConfig;

  parse_config("../config/io_ext.ini", &lConfig);

  lConfig.busses[0].devices[0].drv_handle->write(0x38, 0xFF);
  printf("Address: 0x%02X\n", lConfig.busses[0].devices[0].address);
  printf("Number of busses: %d\n", lConfig.num_busses);

  i2c_init_fhs(&lConfig);

  // Prepare callbacks structure. We have only one callback, the rest are NULL.
  memset(&callbacks, 0, sizeof(callbacks));
  callbacks.begin_request = begin_request_handler;

  // Start the web server.
  ctx = mg_start(&callbacks, NULL, options);

  // Wait until user hits "enter". Server is running in separate thread.
  // Navigating to http://localhost:8080 will invoke begin_request_handler().
  getchar();

  // Stop the server.
  mg_stop(ctx);

  i2c_close_fhs(&lConfig);

  return 0;
}
