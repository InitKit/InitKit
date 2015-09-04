#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "s16rr.h"
#include "manager.h"

CLIENT * clnt;

void eerror (const char * fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    fprintf (stderr, "s16.systemdr: error: ");
    vfprintf (stderr, fmt, args);
    exit (1);
    va_end (args);
}

#define SetOrFail(Name)                                                        \
    if (!prop_find_name (svc->properties, Name))                               \
        eerror ("required property %s not set\n", Name);

int main (int argc, char * argv[])
{
    int c;
    extern char * optarg;
    extern int optind;
    char * unit = 0;
    svc_t * svc;
    SystemDr * app;

    while ((c = getopt (argc, argv, "u:")) != -1)
    {
        switch (c)
        {
        case 'u':
            unit = optarg;
            break;

        case '?':
            exit (1);
            break;
        }
    }

    if (!unit)
        eerror ("no service-unit specified\n");

    clnt = s16db_context_create ();
    svc = s16db_svc_retrieve_by_name (clnt, unit);

    if (SVC_IS_NULL (svc))
        eerror ("no such service in repository: %s\n", unit);

    SetOrFail ("Service.Type");
    SetOrFail ("Service.ExecStart");

    app = new SystemDr (clnt);
    app->add_svc (svc);

    app->main_loop ();

    return 0;
}
