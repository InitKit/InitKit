enum svc_state_e
{
    OFFLINE,
    ONLINE,
    DEGRADED,
    MAINTENANCE
};

typedef enum svc_state_e svc_state_e;

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
