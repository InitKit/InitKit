#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include "s16.h"
#include "ini.h"

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

    return 1;
}

#define OnError(...)                                                           \
    fprintf (stderr, __VA_ARGS__);                                             \
    goto on_error;
svc_t * parse_unit (int is_systemd, char const * path)
{
    svc_t * new_svc = s16_svc_new ();
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
    {
        char * fmri;

        new_svc->name = strdup (basename (path));
        new_svc->name[(strlen (new_svc->name) - 4)] = '\0';
        asprintf (&fmri, "sdu:/%s", new_svc->name);
        svc_object_set_property_string (new_svc, "S16.FMRI", fmri);
        free (fmri);

        svc_object_set_property_string (new_svc, "S16.Delegate", "systemdr");
    }
    else
    {
        char * fmri;
        char * name;

        if (!prop_find_name (new_svc->properties, "S16.Name"))
        {
            OnError ("error: S16.Name not set");
        }
        name = prop_find_name (new_svc->properties, "S16.Name")->value.pval_u.s;
        asprintf (&fmri, "svc:/%s", name);
        svc_object_set_property_string (new_svc, "S16.FMRI", fmri);
        free (fmri);
        svc_object_set_property_string (new_svc, "S16.Path", path);
        new_svc->name = strdup (name);
    }

    return new_svc;

on_error:
    destroy_svc (new_svc);
    return 0;
}