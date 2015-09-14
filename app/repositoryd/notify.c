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

int i_config_subscribe_status (int p, svc_id_t id, svc_id_t i_id)
{
    sub_config_sub_t * conf;
    subscriber_t * sub = i_subscriber_find_by_port (p);

    if (!sub)
        return 1;

    conf = malloc (sizeof (sub_config_sub_t));
    conf->id = id;
    conf->i_id = i_id;
    conf->status_only = 1;

    config_sub_list_add (sub->config_subs, conf);
    return 0;
}