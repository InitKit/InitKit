#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "manager.h"

void eerror (const char * fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    fprintf (stderr, "s16.systemdr: error: ");
    vfprintf (stderr, fmt, args);
    exit (1);
    va_end (args);
}

int main (int argc, char * argv[])
{
    int c;
    extern char * optarg;
    extern int optind;
    const char * unit = 0;

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

    return 0;
}
