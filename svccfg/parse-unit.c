#include "s16.h"
#include "ini.h"

#define OnError(...)                                                           \
    fprintf (stderr, __VA_ARGS__);                                             \
    goto on_error;

int parse_config_line (void * user, const char * section, const char * name,
                       const char * value)
{
    char * key;
    svc_t * svc = user;

    if (!strlen (name))
        return 1;

    asprintf (&key, "%s.%s", section, name);
    svc_object_set_property_string (svc, key, value);
    free (key);

    return 0;
}

svc_t * parse_unit (int is_systemd, char const * path)
{
    svc_t * new_svc = calloc (1, sizeof (svc_t));
    int inierror = ini_parse (path, parse_config_line, new_svc);

    if (inierror > 0)
    {
        OnError ("Error: failed parsing line %d of unit file %s\n", inierror,
                 path);
    }
    else if (inierror < 0)
    {
        OnError ("Error: failed to read unit file %s\n", path);
    }

    if (is_systemd)
        svc_object_set_property_string (new_svc, "s16.restarter", "systemd");

    return new_svc;

on_error:
    destroy_svc (new_svc);
    return 0;
}
