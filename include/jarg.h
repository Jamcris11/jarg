#ifndef JARG_H
#define JARG_H

#include <string.h>
#include <stdio.h>

#define LEN(x) sizeof(x)/sizeof(x[0])

struct arg {
	char* identifier;
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
