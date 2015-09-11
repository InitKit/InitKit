/* RestartD needs a configuration repository, but during system initialisation,
 * ConfigD is not running, as RestartD is the 2nd daemon launched after init.
 * As such, the appropriate structures for RestartD to monitor a ConfigD are
 * constructed here and inserted into the restarter.
 * No Service ID or Instance ID is set here; we don't use them except for
 * communication with s16.configd, and we will later retrieve unique
 * identifiers from configd when we install into it its own service structure.
 */

svc_t * assemble_configd_svc
{
    svc_t * new_svc = s16_svc_new ();
    char * fmri;

    svc_object_set_property_string (new_svc, "S16.Name", "s16/configd");
    asprintf (&fmri, "svc:/%s", name);
    svc_object_set_property_string (new_svc, "S16.FMRI", fmri);
    free (fmri);

    /* how shall we identify the location of s16 binaries? */
    svc_object_set_property_string (
        new_svc, "Method.Start",
        "out/freebsd.amd64/debug/stage/bin/s16.configd");
    new_svc->name = strdup (name);
    s16_svc_new_default_inst (new_svc);
}
