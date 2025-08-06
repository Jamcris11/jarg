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
		int* it, 
		int jarg_args_len, 
		void (*unrecognised_handle)(char*), 
		int argc, 
		char** argv
) {
	const struct arg* jarg;

	for ( int i = 0; i < jarg_args_len; i++ ) {
		jarg = &jarg_args[i];

		if ( !strcmp(argv[(*it)], jarg->identifier) ) {
			if ( jarg->opt_param != NULL && argc-(*it) <= 1 ) {
				return error(
					"%s requires %s as a parameter", 
					jarg->identifier, 
					jarg->opt_param
				);
			}

			jarg->handle(jarg, jarg->opt_param == NULL ? 1 : 2, &argv[(*it)]);
			(*it) += jarg->opt_param == NULL ? 0 : 1;
			return JARG_SUCCESS;
		}
	}

	return JARG_UNRECOGNISED_CMD;
}

static bool
unrecognised_required_arg(
		const struct arg* arg, 
		int required_args_count, 
		int required_args_handled
) {
	return arg == NULL || 
		( !(arg->flags & JARGF_ANY_COUNT) && 
		required_args_count >= required_args_handled );
}

static bool
registered_arg()
{
	
}

const char*
jarg_error_str()
{
	return &jarg_error_msg;
}

void
jarg_print_usage(char* procname, int jarg_args_len)
{
	printf("usage: %s ", procname);

	for ( int i = 0; i < jarg_args_len; i++ ) {
		const struct arg* arg = &jarg_args[i];

		if ( (arg->flags & JARGF_REQUIRED) == false ) {
			printf("[");
		}

		printf("%s", arg->identifier);

		if ( arg->opt_param != NULL ) {
			printf(" %s", arg->opt_param);
		}

		if ( arg->flags & JARGF_ANY_COUNT ) {
			printf("...");
		}

		if ( (arg->flags & JARGF_REQUIRED) == false ) {
			printf("]");
		}

		printf(" ");
	}

	printf("\n");
}

static enum jarg_result
check_args_valid(int jarg_args_len)
{
	const struct arg* jarg;
	for ( int i = 0; i < jarg_args_len; i++ ) {
		jarg = &jarg_args[i];
		if ( jarg->opt_param != NULL 
			&& jarg->flags & JARGF_ANY_COUNT 
		) {
			return error(
				"arg <%s> - opt_param and JARGF_ANY_COUNT cannot be used together", 
				jarg_args[i].identifier
			);
		}

		// If JARGF_ANY_COUNT is not the last argument
		if ( jarg->flags & JARGF_ANY_COUNT && i != jarg_args_len-1 ) {
			return error(
				"arg <%s> - JARGF_ANY_COUNT can only be used on the last defined argument", 
				jarg_args[i].identifier
			);
		}

		if ( jarg->flags & JARGF_OPT && jarg->flags & JARGF_REQUIRED ) {
			return error(
				"arg <%s> - JARGF_OPT and JARGF_REQUIRED can not be used together", 
				jarg_args[i].identifier
			);
		}
	}

	return JARG_SUCCESS;
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

	if ( !check_args_valid(jarg_args_len) ) {
		return JARG_ERROR;
	}

	for ( int i = 1; i < argc; i++ ) {
		bool arg_recognised = false;

		const enum jarg_result res = 
			handle_arg(&i, jarg_args_len, unrecognised_handle, argc, argv);

		if ( res == JARG_ERROR ) {
			return JARG_ERROR;
		} else if ( res == JARG_SUCCESS ) {
			arg_recognised = true;
		}

		if ( arg_recognised == false && unrecognised_handle != NULL ) {
			const struct arg* rarg = 
				get_required_arg(jarg_args_len, required_handled);
			
			if ( unrecognised_required_arg(rarg, required_handled, required_args) ) {
				unrecognised_handle(argv[i]);
			} else {
				rarg->handle(rarg, rarg->opt_param == NULL ? 1 : 2, &argv[i]);
			}
			
			required_handled++;
		}
	}

	if ( required_handled < required_args ) {
		const struct arg* rarg = 
			get_required_arg(jarg_args_len, required_handled);
		return error("%s is a required argument", rarg->identifier);
	}

	return JARG_SUCCESS;
}

/*
arg [OPTIONS] FILE [FILES...]

first unrecognised option -> FILE
any subsequent options -> [FILES...]
*/
