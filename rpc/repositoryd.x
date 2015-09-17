%#include "s16.h"

program S16_REPOSITORYD_PROG 
{
    version S16_REPOSITORYD_V1
    {
        /* Service-related methods. */

        /* Inserts a new, empty service.
         * string => service name
         * Returns service ID, or 0 for fail. */
        svc_id_t svc_insert(string) = 1;

        /* Installs a service structure.
         * rpc_svc_t -> service structure
         * Returns service ID, or 0 for fail. */
        svc_id_t svc_install(rpc_svc_t) = 2;

        /* Deletes a service.
         * svc_id_t => the service ID to delete
         * Returns 0 for success, or 1 for fail. */
        int svc_delete(svc_id_t) = 3;

        /* Retrieves a service.
         * svc_id_t => service ID for service to retrieve
         * Returns the service structure, or a null pointer for fail. */
        rpc_svc_t svc_retrieve(svc_id_t) = 4;

        /* Retrieves a service by name.
         * string => name of the service to retrieve
	* Returns the service structure, or a null pointer for fail. */
        rpc_svc_t svc_retrieve_by_name(string) = 5;

        /* Retrieves all services.
         * Returns an array of services. */
        rpc_svc_array_t svc_retrieve_all() = 6;

        /* Sets an integer property on a service.
         * svc_id_t => service ID
         * string => key
         * hyper => value
         * Returns 0 for success, or 1 for fail. */
        int svc_set_property_int(svc_id_t, string, hyper) = 7;

        /* Sets a string property on a service.
         * svc_id_t => service ID
         * string => key
         * string => value
         * Returns 0 for success, or 1 for fail. */
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
         * Returns 0 for success, or 1 for fail. */
        int instance_set_property_int(svc_id_t, svc_id_t, string, hyper) = 11;

        /* Sets a string property on an instance.
         * svc_id_t => service ID
         * svc_id_t => instance ID
         * string => key
         * string => value
         * Returns 0 for success, or 1 for fail. */
        int instance_set_property_string(svc_id_t, svc_id_t, string, string) = 12;

        /* Sets the status for an instance.
         * svc_id_t => service ID
         * svc_id_t => instance ID
         * svc_state_e => status
        * Returns 0 for success, or 1 for fail. */
        int instance_set_status(svc_id_t, svc_id_t, svc_state_e) = 13;

        /* Update-notification methods. */

        /* Registers interest for update-notification with repositoryd.
         * It is expected that this is called before you try to subscribe to
         * any events.
         * Returns either a port number to listen on for notifications
         * (no notifications will be delivered until you subscribe to them
         * explicitly) and to identify yourself in further calls, or 0 for 
         * fail. */
         int config_register() = 20;

        /* This is similar to the above, but is used if you have already set
         * a suitable port.
         * Make sure your port is acceptable.
         * int => port */
        void config_register_port(int) = 21;

        /* Subscribes to updates for an instances' status.
         * int => your subscription port (for identifying which sub you are)
         * svc_id_t => service ID
         * svc_id_t => instance ID
         * If the service ID is 0, updates for all services are delivered.
         * Returns 0 for success, or 1 for fail. */
        int config_subscribe_status(int, svc_id_t, svc_id_t) = 22;

        /* Subscribes for delivery of newly-installed services.
         * int => subscription port
         * Returns 0 for success, or 1 for fail. */
        int config_subscribe_services(int) = 23;
    } = 1;
} = 0x261e712e;
