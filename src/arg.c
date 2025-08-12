#include "jarg.h"

static char jarg_error_msg;

static bool
is_opt(char* arg, int arglen)
{
	if ( arglen <= 0 || arg[0] != '-' ) { return false; }
	return true;
}

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

static const struct jarg*
get_required_arg(int jarg_args_len, int nth)
{
	for ( int i = 0; i < jarg_args_len; i++ ) {
		if ( jarg_args[i].flags & JARGF_REQUIRED || jarg_args[i].flags & JARGF_ANY_COUNT ) {
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

static bool
unrecognised_required_arg(
		const struct jarg* arg, 
		const int required_args_count, 
		const int required_args_handled
) {
	return arg == NULL || 
		( !(arg->flags & JARGF_ANY_COUNT) && 
		required_args_count >= required_args_handled );
}

static enum jarg_result
handle_arg(
		int* it, 
		int jarg_args_len, 
		enum jarg_result (*unrecognised_handle)(char*),
		int* required_handled,
		const int required_args_count,
		int argc, 
		char** argv
) {
	const struct jarg* arg;
	bool arg_recognised = false;

	for ( int i = 0; i < jarg_args_len; i++ ) {
		arg = &jarg_args[i];

		if ( !strcmp(argv[(*it)], arg->identifier) && arg->flags & JARGF_OPT ) {
			if ( arg->opt_param != NULL && argc-(*it) <= 1 ) {
				return error(
					"%s requires %s as a parameter", 
					arg->identifier, 
					arg->opt_param
				);
			}

			if ( arg->flags & JARGF_USAGE ) {
				jarg_print_usage(argv[0], jarg_args_len);
				return JARG_QUIT_EARLY;
			}

			arg->handle(arg, arg->opt_param == NULL ? 1 : 2, &argv[(*it)]);
			(*it) += arg->opt_param == NULL ? 0 : 1;
			return JARG_SUCCESS;
		}
	}

	/*## Arg not recognised ##*/

	const struct jarg* rarg = 
		get_required_arg(jarg_args_len, *required_handled);
		
	if ( unrecognised_handle != NULL 
		&& unrecognised_required_arg(rarg, *(const int*)(required_handled), required_args_count) 
	) {
		return unrecognised_handle(argv[*it]);
	} else if ( is_opt(argv[*it], strlen(argv[*it])) ) {
		return error("unrecognised option %s", argv[*it]);
	} else {
		rarg->handle(rarg, rarg->opt_param == NULL ? 1 : 2, &argv[*it]);
	}
		
	(*required_handled)++;
	return JARG_SUCCESS; 
}

static enum jarg_result
check_args_valid(int jarg_args_len)
{
	const struct jarg* arg;
	for ( int i = 0; i < jarg_args_len; i++ ) {
		arg = &jarg_args[i];
		if ( arg->opt_param != NULL 
			&& arg->flags & JARGF_ANY_COUNT 
		) {
			return error(
				"arg <%s> - opt_param and JARGF_ANY_COUNT cannot be used together", 
				arg->identifier
			);
		}

		// If JARGF_ANY_COUNT is not the last argument
		if ( arg->flags & JARGF_ANY_COUNT && i != jarg_args_len-1 ) {
			return error(
				"arg <%s> - JARGF_ANY_COUNT can only be used on the last defined argument", 
				arg->identifier
			);
		}

		if ( arg->flags & JARGF_OPT && arg->flags & JARGF_REQUIRED ) {
			return error(
				"arg <%s> - JARGF_OPT and JARGF_REQUIRED can not be used together", 
				arg->identifier
			);
		}
	}

	return JARG_SUCCESS;
}

const char*
jarg_error_str()
{
	return &jarg_error_msg;
}

void
jarg_print_usage(char* procname, int jarg_args_len)
{
	int longest_arg = 0;
	char buffer[64];

	printf("usage: %s ", procname);

	for ( int i = 0; i < jarg_args_len; i++ ) {
		const struct jarg* arg = &jarg_args[i];

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

	for ( int i = 0; i < jarg_args_len; i++ ) {
		const struct jarg* arg = &jarg_args[i];
		int current_arg = strlen(arg->identifier);

		if ( arg->opt_param != NULL ) {
			current_arg += 1 + strlen(arg->opt_param);
		}

		if ( longest_arg < current_arg ) {
			longest_arg = current_arg + 1;
		}
	}

	for ( int i = 0; i < jarg_args_len; i++ ) {
		const struct jarg* arg = &jarg_args[i];
		if ( (arg->flags & JARGF_OPT) == 0 ) {
			continue;
		}

		if ( arg->description == NULL ) {
			continue;
		}

		// TODO: needs evenly tabbed (spaced) columns;

		if ( arg->opt_param != NULL ) {
			strcat(buffer, arg->identifier);
			strcat(buffer, " ");
			strcat(buffer, arg->opt_param);
			printf("\t%-*s- %s\n", longest_arg, buffer, arg->description);
			memset(buffer, 0, sizeof(buffer)/sizeof(buffer[0]));
		} else {
			printf("\t%-*s- %s\n", longest_arg, arg->identifier, arg->description);
		}
	}
}

enum jarg_result
jarg_handle_args(
		int jarg_args_len, 
		enum jarg_result (*unrecognised_handle)(char*), 
		int argc,
		char** argv
) {
	const int required_args_count = get_required_args_count(jarg_args_len);
	static int required_handled;

	required_handled = 0;

	if ( !check_args_valid(jarg_args_len) ) {
		return JARG_ERROR;
	}

	for ( int i = 1; i < argc; i++ ) {
		const enum jarg_result res = 
			handle_arg(&i, 
					jarg_args_len, 
					unrecognised_handle, 
					&required_handled,
					required_args_count,
					argc, 
					argv);

		if ( res == JARG_ERROR ) {
			return JARG_ERROR;
		} else if ( res == JARG_QUIT_EARLY ) {
			return JARG_QUIT_EARLY;
		}
	}

	if ( required_handled < required_args_count ) {
		const struct jarg* rarg = 
			get_required_arg(jarg_args_len, required_handled);
		return error("%s is a required argument", rarg->identifier);
	}

	return JARG_SUCCESS;
}
