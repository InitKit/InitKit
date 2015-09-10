program S16_REPOSITORYD_PROG 
{
    version S16_REPOSITORYD_V1
    {
        /* Service-related methods. */

        /* Inserts a new, empty service.
         * string => service name
         * Returns service ID, or 0 for fail. */
        svc_id_t svc_insert(string) = 1;

        svc_id_t svc_install(rpc_svc_t) = 2;
        int svc_delete(svc_id_t) = 3;
        rpc_svc_t svc_retrieve(svc_id_t) = 4;
        rpc_svc_t svc_retrieve_by_name(string) = 5;
        rpc_svc_array_t svc_retrieve_all() = 6;

        /* Sets an integer property on a service.
         * svc_id_t => service ID
         * string => key
         * hyper => value
         * Returns 1 for success, or 0 for fail. */
        int svc_set_property_int(svc_id_t, string, hyper) = 7;

        /* Sets a string property on a service.
         * svc_id_t => service ID
         * string => key
         * string => value
         * Returns 1 for success, or 0 for fail. */
        int svc_set_property_string(svc_id_t, string, string) = 8;

        /* Instance-related methods. */

        /* Create a new instance of a service.
         * svc_id_t => service ID
         * string => instance name
         * Returns instance number, or 0 for fail. */
        svc_id_t instance_create(svc_id_t, string) = 9;

        /* Retrieve a service instance.
         * svc_id_t => service ID
         * svc_id_t => instance ID
         * Returns pointer to instance, or 0 for fail. */
        rpc_svc_instance_t instance_retrieve(svc_id_t, svc_id_t) = 10;

        /* Sets an integer property on an instance.
         * svc_id_t => service ID
         * svc_id_t => instance ID
         * string => key
         * hyper => value
         * Returns 1 for success, or 0 for fail. */
        int instance_set_property_int(svc_id_t, svc_id_t, string, hyper) = 11;

        /* Sets a string property on an instance.
         * svc_id_t => service ID
         * svc_id_t => instance ID
         * string => key
         * string => value
         * Returns 1 for success, or 0 for fail. */
        int instance_set_property_string(svc_id_t, svc_id_t, string, string) = 12;
    } = 1;
} = 0x261e712e;
