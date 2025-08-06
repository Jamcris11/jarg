#ifndef JARG_H
#define JARG_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define LEN(x) sizeof(x)/sizeof(x[0])

enum arg_flag
{
	JARGF_REQUIRED	= 1 << 0,
	JARGF_ANY_COUNT = 1 << 1,
	JARGF_OPT		= 1 << 2,
};

struct arg
{
	char* identifier;
	char* opt_param;
	enum arg_flag flags;
	void (*handle)(const struct arg* jarg, int argc, char** argv);
};

extern const struct arg jarg_args[];

const char*	jarg_error_str();

void	jarg_print_usage(char* procname, int jarg_args_len);

int	jarg_handle_args(
		int jargs_args_len, 
		void (*unrecognised_handle)(char*), 
		int argc, 
		char** argv);

#endif // JARG_H
