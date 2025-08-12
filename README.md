# jarg - Jim's args
# A simple C library for argument parsing 

## Build
```
make
```
## Install
```
sudo make install
```

## Usage
Your program must define `const struct arg jarg_args[]` or the linker will throw an undefined error.
It will look something like this:
```c
/* example.h */
#include <jarg/jarg.h>

/* example.c */
const struct arg jarg_args[] = {
/*    id,       opt_param,  example,        flags,                          handle      */
    { "-f",     "file",     "e.g. desc",    JARGF_OPT,                      handle_arg_f },
    { "-x",     NULL,       "e.g. desc",    JARGF_OPT,                      handle_arg_x },
    { "FILE",   NULL,       "e.g. desc",    JARGF_REQUIRED|JARGF_ANY_COUNT, handle_arg_file },
};
```

```yaml
flags:
    - JARGF_ANY_COUNT: allows unlimited arguments, only can be used on the last arg
    - JARGF_REQUIRED : make the argument a requirement, throws error if not provided, does not work with JARGF_OPT
    - JARGF_OPT      : option flag, for cmds like -x -d -y etc...
```

```c
handle -> void func_name(const struct arg* arg, int argc, char** argv);
```

`jarg_print_usage(char* procname, int jarg_args_len)` can be used to show cmd help.
```c
if ( argc < 2 ) {
    jarg_print_usage(argv[0], sizeof(jarg_args)/sizeof(jarg_args[0]));
    return 0;
}
```

In your program you can then call `jarg_handle_args(...)` which takes parameters:
```c
int jargs_args_len,                 // jarg_args length
void (*unrecognised_handle)(char*), // a pointer to a function to handle unrecognised arguments
int argc,                           // argc
char** argv                         // argv
```
`jarg_handle_args(...)` will return JARG_SUCCESS or JARG_ERROR depending on outcome.

If JARG_ERROR returned, jarg_error_str() will return the error string.

```c
if ( jarg_handle_args(
        sizeof(jarg_args)/sizeof(jarg_args[0]), 
        unrecognised_arg, 
        argc, 
        argv
    ) == JARG_ERROR 
) {
        fprintf(stderr, "Error: %s\n", jarg_error_str());
        return 1;
}
```

See `tests/main.c` for a more complete example.
