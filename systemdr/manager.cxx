#include <strings.h>
#include "manager.h"

#define CompareType(typ)                                                       \
    !strcasecmp (svc_object_get_property_string (svc, "Service.Type"), typ)

SvcManager::SvcManager (svc_t * svc) : m_svc (svc), m_state_factory (svc, *this)
{
    if (CompareType ("simple"))
        m_type = SIMPLE;
    else if (CompareType ("forking"))
        m_type = FORKING;
    else
        printf ("Fail: service type unknown\n");

    if (svc_object_get_property_string (svc, "Service.ExecStartPre"))
        m_state_stack.push_back (m_state_factory.new_start_pre ());
}
