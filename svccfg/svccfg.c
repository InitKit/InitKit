#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "s16db.h"
#include "svccfg.h"

CLIENT * clnt;

typedef enum
{
    IMPORT_SYSTEMD,
} svccfg_mode;

void unexpected_option (const char * opt)
{
    fprintf (stderr, "svccfg: unexpected option `%s'\n", opt);
    exit (1);
}

void eerror (const char * str)
{
    fprintf (stderr, "svccfg: error: %s", str);
    exit (1);
}

int main (int argc, char * argv[])
{
    int c;
    extern char * optarg;
    extern int optind;
    svccfg_mode mode;
    if (argc < 2)
    {
        fprintf (stderr, "svccfg: expected option\n");
        exit (1);
    }

    if (!strcasecmp (argv[1], "import-systemd"))
        mode = IMPORT_SYSTEMD;
    else
        goto mode_unknown;

    clnt = s16db_context_create ();
    s16db_svc_insert (clnt, "Hello");

    svc_t * box = s16db_svc_retrieve_all (clnt), *iter, *tmp;
    HASH_ITER (hh, box, iter, tmp)
    {
        printf ("ID: %d Service: %s\n", iter->id, iter->name);
    }

    optind++;

    switch (mode)
    {
    case IMPORT_SYSTEMD:
    {
        const char * manifest = 0;
        svc_t * newSvc;

        while ((c = getopt (argc, argv, "m:")) != -1)
        {
            switch (c)
            {
            case 'm':
                manifest = optarg;
                break;
            case '?':
                unexpected_option (optarg);
                break;
            }
        }

        if (!manifest)
            eerror ("import manifest mode, but no manifest specified\n");

        newSvc = parse_systemd_unit (manifest);
        if (!newSvc)
            exit (1);

        s16db_svc_install (clnt, newSvc);
        printf ("installed manifest <%s>");

        return 0;

        break;
    }
    }

    s16db_context_destroy (clnt);
    return 0;

mode_unknown:
    fprintf (stderr, "svccfg: unknown operation: %s\n", argv[1]);
}
