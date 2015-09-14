#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <getopt.h>
#include "s16db.h"
#include "svccfg.h"

CLIENT * clnt;

typedef enum
{
    IMPORT,
} svccfg_mode;

void eerror (const char * fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    fprintf (stderr, "svccfg: error: ");
    vfprintf (stderr, fmt, args);
    exit (1);
    va_end (args);
}

int main (int argc, char * argv[])
{
    int c;
    extern char * optarg;
    extern int optind;
    svccfg_mode mode;
    if (argc < 2)
        eerror ("svccfg: expected option\n");

    if (!strcasecmp (argv[1], "import"))
        mode = IMPORT;
    else
        eerror ("svccfg: unknown operation: %s\n", argv[1]);

    clnt = s16db_context_create ();
    optind++;

    switch (mode)
    {
    case IMPORT:
    {
        int is_systemd = 0;
        const char * manifest = 0;
        svc_t * newSvc;

        while ((c = getopt (argc, argv, "m:s")) != -1)
        {
            switch (c)
            {
            case 'm':
                manifest = optarg;
                break;
            case 's':
                is_systemd = 1;
                break;

            case '?':
                exit (1);
                break;
            }
        }

        if (!manifest)
            eerror ("import manifest mode, but no manifest specified\n");

        newSvc = parse_unit (is_systemd, manifest);
        if (!newSvc)
            exit (1);

        svc_id_t * ret = s16db_svc_install (clnt, newSvc);
        if (ret)
            printf ("installed manifest for service <%s> %d\n", newSvc->name,
                    *ret);
        else
            fprintf (stderr, "failed to install manifest for service <%s>",
                     newSvc->name);
        return 0;

        break;
    }
    }

    /* this will be moved to another utility */
    svc_list box = s16db_svc_retrieve_all (clnt);
    for (svc_list_iterator it = svc_list_begin (box); it != NULL;
         svc_list_iterator_next (&it))
    {
        printf ("Service: %s\n", it->val->name);
        printf ("  ID: %d\n", it->val->id);
        printf ("  Properties:\n");
        for (prop_list_iterator itp = prop_list_begin (it->val->properties);
             itp != NULL; prop_list_iterator_next (&itp))
        {
            printf ("    %s: %s\n", itp->val->name, itp->val->value.pval_u.s);
        }
    }

    s16db_context_destroy (clnt);
    return 0;
}
