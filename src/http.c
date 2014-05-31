/* standard includes */
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <syslog.h>

/* 3rd party libraries */

/* own includes */
#include "http.h"

#include "io.h"

#define BUF_LEN 1024
#define URI_DELIM "/"

typedef enum
{
	REQUEST_ERR, REQUEST_GET, REQUEST_SET,
} request_type;

typedef enum
{
	STAGE_BUS = 0, STAGE_DEV, STAGE_VALUE
} uri_stages;

/* static function declarations */
static int begin_request_handler(struct mg_connection *conn);
static void print_debug_request(const char* uri, io_data* parts,
		request_type type);
static request_type parse_uri(const char* uri, io_data* parts);

struct mg_context* start_http_server(const char *http_options[],
		io_config* bus_config)
{
	struct mg_callbacks callbacks;

	// Prepare callbacks structure. We have only one callback, the rest are NULL.
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.begin_request = begin_request_handler;

	return mg_start(&callbacks, bus_config, http_options);

}

/* This function will be called by mongoose on every new request. */
static int begin_request_handler(struct mg_connection *conn)
{
	const struct mg_request_info *request_info = mg_get_request_info(conn);
	char content[BUF_LEN] =
	{ '\0' };
	io_config* io_bus_config = (io_config*) request_info->user_data;
	io_data parts;
	request_type type = REQUEST_ERR;

	/* process request parameters */
	type = parse_uri(request_info->uri, &parts);

#if 1
	print_debug_request(request_info->uri, &parts, type);
#endif

	/* forward received values to I2C subsystem */
	if (type != REQUEST_ERR)
	{
		perform_io(io_bus_config, &parts,
				type == REQUEST_GET ? CMD_READ : CMD_WRITE, io_data_to_json,
				error_to_json, content,
				BUF_LEN);

	}
	else
	{
		error_to_json("Invalid request type", content, BUF_LEN);
	}

	/* Send HTTP reply to the client */
	mg_printf(conn, "HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: %zu\r\n"
			"\r\n"
			"%s", strnlen(content, BUF_LEN), content);

	/*
	 * Returning non-zero tells mongoose that our function has
	 * replied to the client, and mongoose should not send client
	 * any more data.
	 */
	return 1;
}

int stop_http_server(struct mg_context* context)
{
	/* Stop the server. */
	mg_stop(context);

	return EXIT_SUCCESS;
}

int io_data_to_json(io_data* data, char* buf, int buf_size)
{
	return snprintf(buf, buf_size, "callback({ "
			"\"bus_idx\": \"%d\", "
			"\"dev_idx\": %d, "
			"\"value\": \"%x\""
			" })", data->idx_bus, data->idx_dev, data->value);
}

int error_to_json(char* error_msg, char* buf, int buf_size)
{
	return snprintf(buf, buf_size, "{ "
			"\"msg\": \"%s\""
			" }", error_msg);
}

static request_type parse_uri(const char* uri, io_data* parts)
{
	request_type result = REQUEST_ERR;

	char buf_uri[BUF_LEN];

	char* rest;
	char* token = NULL;
	unsigned cnt_stage = 0;
	long number = 0;
	char* endptr = NULL;
	int base = 10;

	/* circumvent const => unconst cast */
	strncpy(buf_uri, uri, BUF_LEN);

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

static void print_debug_request(const char* uri, io_data* parts,
		request_type type)
{
	char message[BUF_LEN] =
	{ '\0' };
	int cnt = 0;

	cnt += snprintf(message, BUF_LEN, "URI: %s, ", uri);
	switch (type)
	{
	case REQUEST_GET:
		cnt += snprintf(message + cnt, BUF_LEN, "Request: GET, ");
		break;
	case REQUEST_SET:
		cnt += snprintf(message + cnt, BUF_LEN, "Request: SET, ");
		break;
	default:
		cnt += snprintf(message + cnt, BUF_LEN, "Request: ERR");
		break;
	}
	if (REQUEST_ERR != type)
	{
		cnt += snprintf(message + cnt, BUF_LEN, "Bus: %d, Device: %d",
				parts->idx_bus, parts->idx_dev);
		if (REQUEST_SET == type)
		{
			cnt += snprintf(message + cnt, BUF_LEN, ", Value: 0x%02X",
					parts->value);
		}
	}
	syslog(LOG_INFO, "%s", message);
}
