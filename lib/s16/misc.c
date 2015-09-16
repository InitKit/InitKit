#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void perror_fatal (const char * str)
{
    perror (str);
    exit (EXIT_FAILURE);
}

void error_fatal (const char * fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    vfprintf (stderr, fmt, args);
    va_end (args);
    exit (EXIT_FAILURE);
}