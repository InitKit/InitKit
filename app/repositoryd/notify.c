#include "internal.h"
#include "config-subscriber_rpc.h"
#include "s16db.h"

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

int i_config_register_port (int port)
{
    subscriber_t * new_sub = new_subscriber (port);
    subscriber_list_add (RD.subscribers, new_sub);
    return 0;
}

int i_config_subscribe_status (int p, svc_id_t id, svc_id_t i_id)
{
    sub_config_sub_t * conf;
    subscriber_t * sub = i_subscriber_find_by_port (p);

    if (!sub)
        return 1;

    conf = s16mem_alloc (sizeof (sub_config_sub_t));
    conf->id = id;
    conf->i_id = i_id;
    conf->status_only = 1;

    config_sub_list_add (sub->config_subs, conf);
    return 0;
}

int i_config_subscribe_services (int p)
{
    subscriber_t * sub = i_subscriber_find_by_port (p);

    if (!sub)
        return 1;

    sub->services_installed = 1;

    return 0;
}

void notify_1 ()
{
    for (subscriber_list_iterator it = subscriber_list_begin (RD.subscribers);
         it != 0; subscriber_list_iterator_next (&it))
    {
        if (!it->val->clnt)
        {
            struct sockaddr_in addr;
            int sock = RPC_ANYSOCK;
            CLIENT * clnt;

            addr.sin_family = AF_INET;
            addr.sin_port = htons (it->val->port);
            addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);

            clnt = clnttcp_create (&addr, S16_CONFIG_SUBSCRIBER_PROG,
                                   S16_CONFIG_SUBSCRIBER_V1, &sock, 0, 0);
            if (clnt == NULL)
            {
                clnt_pcreateerror ("localhost");
                break;
            }
            it->val->clnt = clnt;
        }
    }
}

void notify_svc (svc_t * svc)
{
    notify_1 ();
    for (subscriber_list_iterator it = subscriber_list_begin (RD.subscribers);
         it != 0; subscriber_list_iterator_next (&it))
    {
        if (!it->val->clnt)
            break;

        config_service_installed_1 (svc_to_rpc_svc (svc), it->val->clnt);
    }
}