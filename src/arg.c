#include "jarg.h"

enum jarg_result
{
	JARG_ERROR=0,
	JARG_SUCCESS,
	JARG_UNRECOGNISED_CMD
};

static char jarg_error_msg;

static int
get_required_args_count(int jarg_args_len)
{
	int sum = 0;
	for ( int i = 0; i < jarg_args_len; i++ ) {
		if ( jarg_args[i].flags & JARGF_REQUIRED & JARGF_ANY_COUNT ) {
			sum++;
			break;
		} else if ( jarg_args[i].flags & JARGF_REQUIRED ) {
			sum++;
		}
	}
	return sum;
}

static const struct arg*
get_required_arg(int jarg_args_len, int nth)
{
	for ( int i = 0; i < jarg_args_len; i++ ) {
		if ( jarg_args[i].flags & JARGF_REQUIRED ) {
			if ( nth != 0 && jarg_args[i].flags & JARGF_ANY_COUNT == 0 ) {
				nth--;
				continue;
			}

			return &jarg_args[i];
		}
	}

	return NULL;
}

static enum jarg_result
error(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsprintf(&jarg_error_msg, fmt, args);
	va_end(args);

	return JARG_ERROR;
}

static int
handle_arg(
		int* i, 
		int j, 
		void (*unrecognised_handle)(char*), 
		int argc, 
		char** argv
) {
	if ( !strcmp(argv[(*i)], jarg_args[j].identifier) ) {
		if ( jarg_args[j].required_params > argc-((*i)+1) ) {
			return error(
				"%s requires %d parameters, %d given.", 
				jarg_args[j].identifier, 
				jarg_args[j].required_params,
				argc-((*i)+1)
			);
		}

		jarg_args[j].handle(jarg_args[j].required_params+1, &argv[((*i))]);
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
	int required_args = get_required_args_count(jarg_args_len);
	int required_handled = 0;
	for ( int i = 1; i < argc; i++ ) {
		bool cmd_matched = false;
		for ( int j = 0; j < jarg_args_len; j++ ) {
			const enum jarg_result res = 
				handle_arg(&i, j, unrecognised_handle, argc, argv);

			if ( res == JARG_ERROR ) {
				return error(NULL);
			} else if ( res == JARG_SUCCESS ) {
				cmd_matched = true;
				break;
			}
		}

		if ( cmd_matched == false && unrecognised_handle != NULL ) {
			const struct arg* rarg = 
				get_required_arg(jarg_args_len, required_handled);
			
			if ( rarg != NULL && 
				!(rarg->flags & JARGF_ANY_COUNT) && 
				required_handled >= required_args 
			) {	
				unrecognised_handle(argv[i]);
			} else {
				rarg->handle(rarg->required_params+1, &argv[i]);
			}
			
			required_handled++;
		}
	}

	if ( required_handled < required_args ) {
		const struct arg* rarg = get_required_arg(jarg_args_len, required_handled);
		return error("%s is a required argument", rarg->identifier);
	}

	return JARG_SUCCESS;
}

/*
arg [OPTIONS] FILE [FILES...]

first unrecognised option -> FILE
any subsequent options -> [FILES...]
*/
