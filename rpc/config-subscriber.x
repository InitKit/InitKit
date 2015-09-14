program S16_CONFIG_SUBSCRIBER_PROG 
{
    version S16_CONFIG_SUBSCRIBER_V1
    {
	int config_instance_state(svc_id_t, svc_id_t, svc_state_e) = 1;
    } = 1;
} = 0x261e712d;
