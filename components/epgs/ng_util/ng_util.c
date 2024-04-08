#include "ng_util.h"
#include "epgs_wrapper.h"

char* strtok_r(
    char *str, 
    const char *delim, 
    char **nextp)
{
    char *ret;

    if (str == NULL)
    {
        str = *nextp;
    }

    str += ng_strspn(str, delim);

    if (*str == '\0')
    {
        return NULL;
    }

    ret = str;

    str += ng_strcspn(str, delim);

    if (*str)
    {
        *str++ = '\0';
    }

    *nextp = str;

    return ret;
}
