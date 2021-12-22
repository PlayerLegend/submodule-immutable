#ifndef FLAT_INCLUDES
#define FLAT_INCLUDES
#include "../range/def.h"
#endif

typedef struct immutable_namespace immutable_namespace;
typedef struct {
    const char * text;
}
    immutable_text;

immutable_text immutable_string (immutable_namespace * namespace, const char * input);
immutable_text immutable_string_range (immutable_namespace * namespace, const range_const_char * input);
immutable_text immutable_path (immutable_namespace * namespace, const char * path);
immutable_text immutable_path_range (immutable_namespace * namespace, const range_const_char * path);
