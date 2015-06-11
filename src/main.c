/* standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <asm-generic/errno.h>
#include <io_i2c_linux.h>
#include <libgen.h>

/* included libraries */

/* own functions */
#include "parse_config.h"
#include "http.h"
#include "io.h"

#define MAX_PATH_LEN 255
#define DEFAULT_CONFIG_PATH "config/io_ext.ini"
#define DAEMON_NAME "switcher"
#define PID_FILE "switcher.pid"
#define PID_DIR "/var/run/switcher"
#define WORK_DIR "/"

extern char* optarg;

static struct mg_server* http_server = NULL;
static io_config io_configuration;
static char *http_options[MAX_NUM_CONF] =
{ NULL };
static FILE* stream_pid = NULL;
static int fh_pid = -1;

static void signal_handler(int sig);
static void daemonize(void);

/*
 * Useful hints:
 *   * http://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
 *   * http://stackoverflow.com/questions/5339200/how-to-create-a-single-instance-application-in-c
 */

int main(int argc, char* argv[])
{
	int option = -1;
	char config_path[STR_CONF_LEN];
	int cnt_option = 0;
	uint8_t run_as_daemon = 0;

#define ADD_OPTION_ELEMENT(option) \
    http_options[cnt_option] = (char*)malloc(STR_CONF_LEN); \
    strncpy(http_options[cnt_option], option, STR_CONF_LEN); \
    cnt_option++;

	ADD_OPTION_ELEMENT("listening_port")
	ADD_OPTION_ELEMENT("8080")

	if (signal(SIGINT, signal_handler) == SIG_ERR
			|| signal(SIGTERM, signal_handler) == SIG_ERR)
	{
		fprintf(stderr, "Could not install signal handler for SIGINT.\n");
		return 1;
	}

	strncpy(config_path, DEFAULT_CONFIG_PATH, STR_CONF_LEN);
	while ((option = getopt(argc, argv, "hc:p:d")) != -1)
	{
		switch (option)
		{
		case 'c':
			strncpy(config_path, optarg, STR_CONF_LEN);
			break;
		case 'p':
			strncpy(http_options[1], optarg, STR_CONF_LEN);
			break;
		case 'd':
			run_as_daemon = 1;
			break;
		default:
			fprintf(stderr, "Usage: %s [-h] [-c config-file] [-p port]\n",
					argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (conf_init(&io_configuration) == EXIT_FAILURE)
	{
		fprintf(stderr,
				"Error initializing the bus configuration data structure.\n");
		exit(EXIT_FAILURE);
	}

	if (parse_config(config_path, &io_configuration) == EXIT_FAILURE)
	{
		fprintf(stderr, "Error parsing the configuration file.\n");
		exit(EXIT_FAILURE);
	}
	else if (validate_config(&io_configuration) == EXIT_FAILURE)
	{
		fprintf(stderr, "Illegal configuration. Please check values.\n");
		exit(EXIT_FAILURE);
	}

	openlog(DAEMON_NAME, LOG_PID, LOG_DAEMON);

	if (run_as_daemon)
	{
		daemonize();
	}

	if (io_init(&io_configuration) == EXIT_FAILURE)
	{
		syslog(LOG_ERR, "Could not initialize IO subsystem.");
		exit(EXIT_FAILURE);
	}

	http_server = start_http_server((const char **) http_options, cnt_option,
			&io_configuration);

	for (;;)
	{
		mg_poll_server(http_server, 1000);
	}

	return EXIT_FAILURE; /* should never be reached */
}

static void signal_handler(int sig)
{
	int ret = 0;
	int cnt = 0;

	if (NULL != http_server)
	{
		syslog(LOG_INFO, "Shutting down HTTP server.");
		stop_http_server(http_server);
		http_server = NULL;
	}

	syslog(LOG_INFO, "Shutting down IO subsystem.");
	io_deinit(&io_configuration);
	conf_deinit(&io_configuration);

	for (cnt = 0; cnt < MAX_NUM_CONF; cnt++)
	{
		if (NULL != http_options[cnt])
		{
			free(http_options[cnt]);
			http_options[cnt] = NULL;
		}
	}

	if (NULL != stream_pid)
	{
		if (flock(fh_pid, LOCK_UN) != 0)
		{
			syslog(LOG_ERR, "Could not unlock PID file.");
		}
		fclose(stream_pid);
		stream_pid = NULL;
	}

	unlink(PID_FILE);
	closelog();

	exit(ret);
}

static void daemonize(void)
{
	int cnt = -1;
	pid_t pid;
	int rc = -1;
	char pid_path[MAX_PATH_LEN] =
	{ '\0' };
	struct stat st_data =
	{ 0 };

	pid = fork();

	if (pid < 0)
	{
		syslog(LOG_ERR, "Could not spawn daemon process. Stage 1.");
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		/* terminate parent process */
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0)
	{
		syslog(LOG_ERR, "Daemon cannot become session leader.");
		exit(EXIT_FAILURE);
	}

	/* ignore child spawning signals */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0)
	{
		syslog(LOG_ERR, "Could not spawn daemon process. Stage 2.");
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		exit(EXIT_SUCCESS);
	}

	umask(0);

	chdir(WORK_DIR);

	for (cnt = sysconf(_SC_OPEN_MAX); cnt > 0; cnt--)
	{
		close(cnt);
	}

	if (snprintf(pid_path, MAX_PATH_LEN, "%s/%s", PID_DIR, PID_FILE) < 0)
	{
		syslog(LOG_ERR, "Could not create complete path of PID file.");
		exit(EXIT_FAILURE);
	}
	if (stat(PID_DIR, &st_data) == -1)
	{
		if (mkdir(PID_DIR, 0700) == -1)
		{
			syslog(LOG_ERR, "Could not create directory for PID file.");
			exit(EXIT_FAILURE);
		}
	}
	fh_pid = open(pid_path, O_CREAT | O_RDWR, 0666);
	rc = flock(fh_pid, LOCK_EX | LOCK_NB);
	if (rc)
	{
		if (EWOULDBLOCK == errno)
		{
			syslog(LOG_ERR,
					"Quitting. There is already another instance running.");
			exit(EXIT_FAILURE);
		}
	}

	stream_pid = fdopen(fh_pid, "r+");
	if (NULL == stream_pid)
	{
		syslog(LOG_ERR, "Could not create stream of PID file handle.");
		exit(EXIT_FAILURE);
	}
	fprintf(stream_pid, "%d\n", getpid());
	fflush(stream_pid);

	syslog(LOG_ERR, "switcher daemon started.\n");
}
