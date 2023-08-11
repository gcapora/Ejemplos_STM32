#ifndef _TINYSH_UTILS_
#define _TINYSH_UTILS_

#ifdef __cplusplus
extern "C" {
#endif

int tinysh_get_arg_int(const int argc, const char **argv, const int index);

float tinysh_get_arg_float(const int argc, const char **argv, const int index);

const char* tinysh_get_arg_string(const int argc, const char **argv, const int index);

#ifdef __cplusplus
}
#endif

#endif /* _TINYSH_UTILS_ */
