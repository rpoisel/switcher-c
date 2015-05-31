/* standard includes */
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <syslog.h>

/* 3rd party libraries */

/* own includes */
#include <trace.h>
#include <http.h>
#include <io.h>

#define BUF_LEN 1024
#define URI_DELIM "/"
#define DEV_DELIM "."

typedef enum
{
	REQUEST_ERR, REQUEST_GET, REQUEST_SET,
} request_t;

typedef enum
{
	STAGE_BUS = 0, STAGE_DEV, STAGE_VALUE
} uri_stage_t;

typedef enum
{
	RC_IO_OK, RC_IO_ERROR
} io_success_t;

/* static function declarations */
static int event_handler(struct mg_connection* conn, enum mg_event ev);
static int http_request_handler(struct mg_connection *conn);
static int websocket_request_handler(struct mg_connection *conn);
static void print_request(const char* uri, io_data* parts, request_t type);
static request_t parse_uri(const char* uri, io_data* parts);
static io_success_t parse_bus(const char* token, idx_t* bus_idx);
static io_success_t parse_dev(char* token, idx_t* dev_idx, idx_t* sub_dev_idx);
static io_success_t parse_value(const char* token, value_t* value);

struct mg_server* start_http_server(const char *http_options[],
		int num_http_options, io_config* bus_config)
{
	int cnt = 0;
	struct mg_server* server = mg_create_server(bus_config, event_handler);
	for (cnt = 0; cnt < num_http_options; cnt += 2)
	{
		mg_set_option(server, http_options[cnt], http_options[cnt + 1]);
	}

	return server;
}

static int event_handler(struct mg_connection* conn, enum mg_event event)
{
	switch (event)
	{
	case MG_REQUEST:
	{
		if (conn->is_websocket)
		{
			return websocket_request_handler(conn);
		}
		else
		{
			return http_request_handler(conn);
		}
	}
	case MG_AUTH:
		return MG_TRUE; /* no authentication necessary at the moment */
	default:
		return MG_FALSE;
	}
	return MG_FALSE;
}

/* This function will be called by mongoose on every new request. */
static int http_request_handler(struct mg_connection *conn)
{
	char content[BUF_LEN] =
	{ '\0' };
	io_config* io_bus_config = (io_config*) conn->server_param;
	io_data parts;
	request_t type = REQUEST_ERR;

	/* process request parameters */
	type = parse_uri(conn->uri, &parts);

	print_request(conn->uri, &parts, type);

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
			"Content-Type: text/javascript\r\n"
			"Content-Length: %zu\r\n"
			"\r\n"
			"%s", strnlen(content, BUF_LEN), content);

	/*
	 * Returning non-zero tells mongoose that our function has
	 * replied to the client, and mongoose should not send client
	 * any more data.
	 */
	return MG_TRUE;
}

static int websocket_request_handler(struct mg_connection *conn)
{
#if 0
	io_config* io_bus_config = (io_config*) conn->server_param;
#endif
	return MG_TRUE;
}

int stop_http_server(struct mg_server* server)
{
	/* Stop the server. */
	mg_destroy_server(&server);

	return EXIT_SUCCESS;
}

int io_data_to_json(io_data* data, char* buf, int buf_size)
{
	return snprintf(buf, buf_size, "callback({ "
			"\"bus_idx\": \"%d\", "
			"\"dev_idx\": %d, "
			"\"dev_sub_idx\": %d, "
			"\"value\": \"%x\""
			" })", data->idx_bus, data->idx_dev, data->idx_sub_dev, data->value);
}

int error_to_json(char* error_msg, char* buf, int buf_size)
{
	return snprintf(buf, buf_size, "{ "
			"\"msg\": \"%s\""
			" }", error_msg);
}

static request_t parse_uri(const char* uri, io_data* parts)
{
	request_t result = REQUEST_ERR;

	char buf_uri[BUF_LEN];

	char* rest;
	char* token = NULL;
	unsigned cnt_stage = 0;

	INIT_IO_DATA(parts)

	/* strtok will modify its buffer, therefore copy it beforehand */
	strncpy(buf_uri, uri, BUF_LEN);

	token = strtok_r(buf_uri, URI_DELIM, &rest);
	while (cnt_stage <= STAGE_VALUE && token != NULL)
	{
		/* assign parsed number to current stage's counterpart in data structure */
		switch (cnt_stage)
		{
		case STAGE_BUS:
			if (parse_bus(token, &(parts->idx_bus)) != RC_IO_OK)
			{
				return REQUEST_ERR;
			}
			break;
		case STAGE_DEV:
			if (parse_dev(token, &(parts->idx_dev), &(parts->idx_sub_dev))
					!= RC_IO_OK)
			{
				return REQUEST_ERR;
			}
			result = REQUEST_GET;
			break;
		case STAGE_VALUE:
			if (parse_value(token, &(parts->value)) != RC_IO_OK)
			{
				return REQUEST_ERR;
			}
			result = REQUEST_SET;
			break;
		default:
			/* illegal stage */
			return REQUEST_ERR;
		}
		++cnt_stage;
		token = strtok_r(NULL, URI_DELIM, &rest);
	}

	return result;
}

static io_success_t parse_bus(const char* token, idx_t* bus_idx)
{
	char* endptr = NULL;
	*bus_idx = strtol(token, &endptr, 10);

	if ((errno == ERANGE && (*bus_idx == LONG_MAX || *bus_idx == LONG_MIN))
			|| endptr == token)
	{
		return RC_IO_ERROR;
	}
	return RC_IO_OK;
}

static io_success_t parse_dev(char* token, idx_t* dev_idx, idx_t* sub_dev_idx)
{
	char* dev_token = NULL;
	char* saveptr = NULL;
	char* endptr = NULL;
	dev_token = strtok_r(token, DEV_DELIM, &saveptr);

	*dev_idx = strtol(dev_token, &endptr, 10);

	if ((errno == ERANGE && (*dev_idx == LONG_MAX || *dev_idx == LONG_MIN))
			|| endptr == token)
	{
		return RC_IO_ERROR;
	}

	dev_token = strtok_r(NULL, DEV_DELIM, &saveptr);
	if (dev_token != NULL)
	{
		*sub_dev_idx = strtol(dev_token, &endptr, 10);

		if ((errno == ERANGE
				&& (*sub_dev_idx == LONG_MAX || *sub_dev_idx == LONG_MIN))
				|| endptr == token)
		{
			return RC_IO_ERROR;
		}
	}

	return RC_IO_OK;
}

static io_success_t parse_value(const char* token, value_t* value)
{
	char* endptr = NULL;
	*value = strtoul(token, &endptr, 16);

	if (*endptr != '\0')
	{
		return RC_IO_ERROR;
	}
	return RC_IO_OK;
}

static void print_request(const char* uri, io_data* parts, request_t type)
{
	char message[BUF_LEN] =
	{ '\0' };
	int cnt = 0;

	switch (type)
	{
	case REQUEST_GET:
		cnt += snprintf(message + cnt, BUF_LEN, "Request: GET, ");
		break;
	case REQUEST_SET:
		cnt += snprintf(message + cnt, BUF_LEN, "Request: SET, ");
		break;
	default:
		cnt += snprintf(message + cnt, BUF_LEN, "Request: ERR, ");
		break;
	}
	cnt += snprintf(message + cnt, BUF_LEN, "URI: %s", uri);
	if (REQUEST_ERR != type)
	{
		cnt += snprintf(message + cnt, BUF_LEN, ", Bus: %d, Device: %d",
				parts->idx_bus, parts->idx_dev);
		if (parts->idx_sub_dev != IDX_INVALID)
		{
			cnt += snprintf(message + cnt, BUF_LEN, ", Sub-Device: %d",
					parts->idx_sub_dev);
		}
		if (REQUEST_SET == type)
		{
			cnt += snprintf(message + cnt, BUF_LEN, ", Value: 0x%02X",
					parts->value);
		}
	}
	syslog(LOG_INFO, "%s", message);
	TRACE(printf("%s\n", message));
}
