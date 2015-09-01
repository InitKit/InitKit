struct svc_s
{
    unsigned long id;
    char * name;
    struct property_s properties<>;
    struct svc_instance_s instances<>;
};

struct svc_instance_s
{
    unsigned int id;
    char * name;

    unsigned int svc_id;
};

enum property_type_e
{
    NUMBER,
    STRING
};

typedef enum property_type_e property_type_t;

union property_value_u switch (property_type_t type)
{
case NUMBER:
    int i;
case STRING:
    string s<>;
};

typedef union property_value_u property_value_t;

enum property_parent_type_e
{
    SVC,
    SVCI,
    PROP
};

typedef enum property_parent_type_e property_parent_type_t;

struct property_s
{
    unsigned int id; /* uthash key */
    string name<>;     /* key, but not uthash key */
    property_value_t value;

    unsigned int parent_id;
};

typedef struct svc_s svc_t;

program S16_REPOSITORYD_PROG 
{
    version ADD_VERS 
    {
        int insert_service(string) = 1;
    } = 1;
} = 0x16147123;
