#include "internal.h"

subscriber_t * i_subscriber_find_by_port (int port)
{
    for (subscriber_list_iterator it = subscriber_list_begin (RD.subscribers);
         it != NULL; subscriber_list_iterator_next (&it))
    {
        if (it->val->port == port)
            return it->val;
    }
    return NULL;
}

int i_config_register ()
{
    subscriber_t * new_sub;
    int randnum = 49152;

    while (i_subscriber_find_by_port (randnum))
    {
        double s = (double)rand () / RAND_MAX;
        randnum = (16384 * s) + 49152;
    }

    new_sub = new_subscriber (randnum);
    subscriber_list_add (RD.subscribers, new_sub);

    return randnum;
}