typedef unsigned long svc_id_t;

struct rpc_svc_s
{
    svc_id_t id;
    char * name;
    struct rpc_property_s properties<>;
    struct rpc_svc_instance_s instances<>;
};

struct rpc_svc_instance_s
{
    unsigned int id;
    char * name;
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
    int i;
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

program S16_REPOSITORYD_PROG 
{
    version S16_REPOSITORYD_V1
    {
        int insert_service(string) = 1;
        int delete_service(svc_id_t) = 2;
        rpc_svc_t retrieve_service(svc_id_t) = 3;
    } = 1;
} = 0x16147123;
