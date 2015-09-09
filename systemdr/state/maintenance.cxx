#include <sys/types.h>
#include <signal.h>
#include "state.h"
#include "manager.h"

MaintenanceState::MaintenanceState (svc_t * svc, SvcManager & manager)
    : SvcState (svc, manager)
{
    if (m_manager.main_pid)
        svc_object_set_property_string (svc, "S16.Status", "maintenance");
}

int MaintenanceState::process_event (pt_info_t * info) {}

int MaintenanceState::loop_iteration () {}
