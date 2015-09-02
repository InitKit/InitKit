#include <stdio.h>
#include "s16db.h"

CLIENT * clnt;

int main (int argc, char * argv[])
{
    clnt = s16db_context_create ();
    s16db_svc_insert (clnt, "Hello");

    svc_t * box = s16db_svc_retrieve_all (clnt), *iter, *tmp;
    HASH_ITER (hh, box, iter, tmp)
    {
        printf ("ID: %d Service: %s\n", iter->id, iter->name);
    }

    s16db_context_destroy (clnt);
    return 0;
}
