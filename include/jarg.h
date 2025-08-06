#ifndef JARG_H
#define JARG_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define LEN(x) sizeof(x)/sizeof(x[0])

enum arg_flag
{
	JARGF_REQUIRED =	1 << 0,
	JARGF_ANY_COUNT =	1 << 1,
};

struct arg
{
	char* identifier;
	enum arg_flag flags;
	void (*handle)(int argc, char** argv);
	int required_params;
};

extern const struct arg jarg_args[];

const char*	jarg_error_str();

int	jarg_handle_args(
		int jargs_args_len, 
		void (*unrecognised_handle)(char*), 
		int argc, 
		char** argv);

#endif // JARG_H
