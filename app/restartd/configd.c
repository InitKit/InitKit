/* RestartD needs a configuration repository, but during system initialisation,
 * ConfigD is not running, as RestartD is the 2nd daemon launched after init.
 * As such, the appropriate structures for RestartD to monitor a ConfigD are
 * constructed here and inserted into the restarter.
 * No Service ID or Instance ID is set here; we don't use them except for
 * communication with s16.configd, and we will later retrieve unique
 * identifiers from configd when we install into it its own service structure.
 */

#include "s16.h"
#include "manager.h"
#include "unit.h"

void install_configd_svc ()
{
    svc_t * new_svc = s16_svc_new ();
    char * fmri;

    new_svc->id = 1;
    new_svc->name = strdup ("s16/configd");
    svc_object_set_property_string (new_svc, "S16.Name", new_svc->name);
    asprintf (&fmri, "svc:/%s", new_svc->name);
    svc_object_set_property_string (new_svc, "S16.FMRI", fmri);
    free (fmri);

    svc_object_set_property_string (new_svc, "Unit.Strategy", "exec");
    svc_object_set_property_string (
        new_svc, "Unit.Description",
        "The S16 Service Configuration Repository.");

    /* how shall we identify the location of s16 binaries? */
    svc_object_set_property_string (new_svc, "Method.Prestart",
                                    "/bin/echo PreStart");
    svc_object_set_property_string (new_svc, "Method.Start", "/bin/sleep 7");
    svc_object_set_property_string (new_svc, "Method.Poststart",
                                    "/bin/echo Execed poststart");
    /* "out/freebsd.amd64/debug/stage/bin/s16.repositoryd");*/
    s16_inst_new_default (new_svc);
    inst_list_lget (new_svc->instances)->id = 1;

    unit_list_add (Manager.units,
                   unit_new (new_svc, inst_list_lget (new_svc->instances)));
}
