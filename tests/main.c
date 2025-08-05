#include <stdio.h>
#include <stdlib.h>
#include "../include/jarg.h"

static void
handle_print(int argc, char** argv)
{
	for ( int i = 0; i < argc; i++ ) {
		printf("%s\n", argv[i]);
	}
}

static void
handle_unrecognised_cmd(char* cmd)
{
	if ( !strcmp(cmd, "beef") ) {
		printf("Bonjour!\n");
	} else {
		fprintf(stderr, "FATAL: unrecognised cmd parameter\n");
		exit(1);
	}
}

const struct arg jarg_args[] = {
	{ "-p", handle_print, 2 },
};

int
main(int argc, char** argv)
{
	if ( !jarg_handle_args(LEN(jarg_args), handle_unrecognised_cmd, argc, argv) ) {
		fprintf(stderr, "Error: failed to handle arguments - %s\n", jarg_error_str());
		return 1;
	}
	return 0;
}
