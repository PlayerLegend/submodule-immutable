#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
char *realpath(const char *path, char *resolved_path);
#define FLAT_INCLUDES
#include "../range/def.h"
#include "immutable.h"

#define TABLE_STRING

#include "../table/table.h"
#include "../table/table-string.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../window/string.h"

struct immutable_namespace
{
    table_string table;
    pthread_mutex_t mutex;
    window_char tmp_string;
};

static immutable_namespace _default_namespace = { .mutex = PTHREAD_MUTEX_INITIALIZER };

immutable_text immutable_string (immutable_namespace * namespace, const char * input)
{
    assert (input);

    if (!namespace)
    {
	namespace = &_default_namespace;
    }
    
    const char * ret;
    pthread_mutex_lock (&namespace->mutex);
    ret = table_string_include (namespace->table, input)->query.key;
    pthread_mutex_unlock (&namespace->mutex);
    return (immutable_text) { ret };
}

immutable_text immutable_path (immutable_namespace * namespace, const char * path)
{
    assert (path);
    
    if (!namespace)
    {
	namespace = &_default_namespace;
    }
    
    const char * ret;
    static char resolved[PATH_MAX];
    assert (path);
    pthread_mutex_lock (&namespace->mutex);
    
    if (realpath (path, resolved))
    {
	ret = table_string_include (namespace->table, resolved)->query.key;
    }
    else
    {
	ret = table_string_include (namespace->table, "")->query.key;
	perror (path);
    }
    pthread_mutex_unlock (&namespace->mutex);
    return (immutable_text) { ret };
}

immutable_text immutable_string_range (immutable_namespace * namespace, const range_const_char * input)
{
    if (!namespace)
    {
	namespace = &_default_namespace;
    }

    window_strcpy_range (&namespace->tmp_string, input);

    assert (*namespace->tmp_string.region.end == '\0');

    return immutable_string (namespace, namespace->tmp_string.region.begin);
}

immutable_text immutable_path_range (immutable_namespace * namespace, const range_const_char * input)
{
    if (!namespace)
    {
	namespace = &_default_namespace;
    }

    window_strcpy_range (&namespace->tmp_string, input);

    assert (*namespace->tmp_string.region.end == '\0');

    return immutable_path (namespace, namespace->tmp_string.region.begin);
}

immutable_namespace * immutable_namespace_new()
{
    return calloc (1, sizeof(immutable_namespace));
}

void immutable_namespace_free (immutable_namespace * target)
{
    table_string_clear (target->table);
    pthread_mutex_destroy (&target->mutex);
    window_clear (target->tmp_string);
    free (target);
}
