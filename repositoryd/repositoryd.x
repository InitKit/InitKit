typedef unsigned long svc_id_t;

struct rpc_svc_s
{
    svc_id_t id;
    string name<>;
    struct rpc_property_s properties<>;
    struct rpc_svc_instance_s instances<>;
};

struct rpc_svc_instance_s
{
    unsigned int id;
    string name<>;
    struct rpc_property_s properties<>;

    svc_id_t svc_id;
};

enum property_type_e
{
    NUMBER,
    STRING
};

typedef enum property_type_e property_type_t;

union pval switch (property_type_t type)
{
case NUMBER:
    hyper i;
case STRING:
    string s<>;
};

typedef union pval pval_t;

enum property_parent_type_e
{
    SVC,
    SVCI,
    PROP
};

typedef enum property_parent_type_e property_parent_type_t;

struct rpc_property_s
{
    unsigned int id; /* uthash key */
    string name<>;     /* key, but not uthash key */
    pval_t value;

    unsigned int parent_id;
};

typedef struct rpc_property_s rpc_property_t;
typedef struct rpc_svc_instance_s rpc_svc_instance_t;
typedef struct rpc_svc_s rpc_svc_t;

typedef rpc_svc_t rpc_svc_array_t<>;

program S16_REPOSITORYD_PROG 
{
    version S16_REPOSITORYD_V1
    {
        /* inserts a service
         * string => service name
         * returns service number or 0 for fail */
        svc_id_t svc_insert(string) = 1;
        svc_id_t svc_install(rpc_svc_t) = 2;
        int svc_delete(svc_id_t) = 3;
        rpc_svc_t svc_retrieve(svc_id_t) = 4;
        rpc_svc_t svc_retrieve_by_name(string) = 5;
        rpc_svc_array_t svc_retrieve_all() = 6;
        int svc_set_property_int(svc_id_t, string, hyper) = 7;
        int svc_set_property_string(svc_id_t, string, string) = 8;
    } = 1;
} = 0x16147123;
