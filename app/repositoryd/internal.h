#ifndef __INTERNAL_H_
#define __INTERNAL_H_

#include <stdlib.h>
#include "s16.h"

/* an entry in the configuration-update subscriptions list */
typedef struct sub_config_sub_s
{
    svc_id_t id;
    svc_id_t i_id;
} sub_config_sub_t;

ListGenForNameType (config_sub, sub_config_sub_t);

typedef struct subscriber_s
{
    int port; /* stored in case we need to reconnect. host is always localhost
               * later we might consider allowing remote subscriptions; but
               * but first, we'd need to secure things. */
    CLIENT * clnt;
    config_sub_list config_subs;
} subscriber_t;

static inline subscriber_t * new_subscriber ()
{
    subscriber_t * ret = malloc (sizeof (subscriber_t));
    ret->port = 0;
    ret->clnt = 0;
    ret->config_subs = List_new ();
    return ret;
}

ListGenForNameType (subscriber, subscriber_t);

typedef struct repositoryd_s
{
    svc_list services;
    subscriber_list subscribers;
} repositoryd_t;

extern repositoryd_t RD;

svc_id_t insert_svc (char const * name);
svc_id_t install_svc (svc_t * svc);
int delete_svc (svc_id_t id);
int count_svcs ();
svc_list retrieve_all_svcs ();
int set_svc_property_int (svc_id_t id, char const * name, long value);
int set_svc_property_string (svc_id_t id, char const * name,
                             char const * value);

#endif
