typedef opaque opaque_array<>;

program S16_RESTARTD_PROG 
{
    version S16_RESTARTD_V1
    {
        /* This applies to all of these four methods:
         * svc_id_t => Service ID
         * svc_id_t => Instance ID
         * Returns 0 for success, 1 for failure. */

        /* Start a service instance. */
        int instance_start (svc_id_t, svc_id_t) = 1;

        /* Stop an instance. */
        int instance_stop (svc_id_t, svc_id_t) = 2;

        /* Reloads an instance and then restarts it. */
        int instance_reload (svc_id_t, svc_id_t) = 3;

        /* Restarts an instance. */
        int instance_restart (svc_id_t, svc_id_t) = 4;

        /* Checks if a message is handled by this restarter.
         * string => message name
         * Returns 1 for yes, 0 for no. */
        int message_available(string) = 5;

        /* Sends a message. 
         * opaque => message
         * Returns response. */
        opaque_array message(opaque_array) = 6;
    } = 1;
} = 0x261e712f;
