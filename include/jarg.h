/// @file jarg.h
#ifndef JARG_H
#define JARG_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

enum jarg_result
{
	JARG_ERROR=0,
	JARG_SUCCESS,
	JARG_UNRECOGNISED_CMD
};

enum jarg_flag
{
	JARGF_REQUIRED	= 1 << 0,
	JARGF_ANY_COUNT = 1 << 1,
	JARGF_OPT		= 1 << 2,
};

struct jarg
{
	char* identifier;
	char* opt_param;
	char* description;
	enum jarg_flag flags;
	void (*handle)(const struct jarg* jarg, int argc, char** argv);
};

extern const struct jarg jarg_args[];

/**
 * Returns jarg_error_msg
 * @param[out] jarg_error_msg
 */
const char*	jarg_error_str();

/**
 * Prints arguments and their descriptions
 * @param[in] procname process name, usually argv[0]
 * @param[in] jarg_args_len
 */
void	jarg_print_usage(char* procname, int jarg_args_len);

/**
 * Parses and handles arguments
 * @param[in] jarg_args_len
 * @param[in] unrecognised_handle 
 * @param[in] argc
 * @param[in] argv
 * @return JARG_SUCCESS on success, JARG_ERROR on failure
 */
enum jarg_result	jarg_handle_args(
	int jargs_args_len, 
	void (*unrecognised_handle)(char*), 
	int argc, 
	char** argv);

#endif // JARG_H
