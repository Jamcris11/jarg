#include "jarg.h"

enum jarg_result
{
	JARG_ERROR=0,
	JARG_SUCCESS,
	JARG_UNRECOGNISED_CMD
};

static char jarg_error_msg;

static int
handle_arg(int* i, int j, void (*unrecognised_handle)(char*), int argc, char** argv)
{
	if ( !strcmp(argv[(*i)], jarg_args[j].identifier) ) {
		if ( jarg_args[j].required_params > argc-((*i)+1) ) {

			sprintf(&jarg_error_msg, 
					"%s requires %d parameters, %d given.", 
					jarg_args[j].identifier, 
					jarg_args[j].required_params,
					argc-((*i)+1));
			return JARG_ERROR;
		}

		jarg_args[j].handle(jarg_args[j].required_params, &argv[((*i)+1)]);
		(*i) += jarg_args[j].required_params;
		return JARG_SUCCESS;
	}

	return JARG_UNRECOGNISED_CMD;
}

const char*
jarg_error_str()
{
	return &jarg_error_msg;
}

int
jarg_handle_args(
		int jarg_args_len, 
		void (*unrecognised_handle)(char*), 
		int argc, 
		char** argv
) {
	for ( int i = 1; i < argc; i++ ) {
		bool anymatch = false;
		for ( int j = 0; j < jarg_args_len; j++ ) {
			const enum jarg_result res = 
				handle_arg(&i, j, unrecognised_handle, argc, argv);
			if ( res == JARG_ERROR ) {
				return JARG_ERROR;
			} else if ( res == JARG_SUCCESS ) {
				anymatch = true;
				break;
			}
		}

		if ( anymatch == false && unrecognised_handle != NULL ) {
			unrecognised_handle(argv[i]);
		}

	}

	return JARG_SUCCESS;
}
