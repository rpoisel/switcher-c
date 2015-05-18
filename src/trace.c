#include <trace.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define ENVVAR_DEBUG         "DEBUG"

static char initialized = 0;

int trace_printf(const char* format, ...)
{
	va_list ap;
	int retVal = 0;
	static char* env_debug = NULL;

	if (!initialized)
	{
		env_debug = getenv(ENVVAR_DEBUG);
	}

	if (NULL == env_debug)
	{
		return 0;
	}
	va_start(ap, format);
	retVal = vfprintf(stdout, format, ap);
	fflush(stdout);
	return retVal;
}
