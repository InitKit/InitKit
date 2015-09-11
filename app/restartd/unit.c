#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "unit.h"

unit_t * unit_new (svc_t * svc, svc_instance_t * inst)
{
#define CompareType(typ)                                                       \
    !strcasecmp (svc_object_get_property_string (svc, "Unit.Form"), typ)

    unit_t * unitnew = malloc (sizeof (unit_t));

    unitnew->svc = svc;
    unitnew->inst = inst;
    unitnew->timer_id = 0;

    if (CompareType ("exec"))
        unitnew->type = T_EXEC;
    else if (CompareType ("forks"))
        unitnew->type = T_FORKS;
    else if (CompareType ("oneshot"))
        unitnew->type = T_ONESHOT;
    else
    {
        fprintf (stderr, "Unit <%s> lacks a known type\n", svc->name);
        return 0;
    }

    unitnew->method[M_PRESTART] =
        svc_object_get_property_string (svc, "Method.Prestart");
    unitnew->method[M_START] =
        svc_object_get_property_string (svc, "Method.Start");
    unitnew->method[M_STOP] =
        svc_object_get_property_string (svc, "Method.Stop");

    return unitnew;
}
