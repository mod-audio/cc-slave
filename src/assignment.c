/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include <string.h>
#include "control_chain.h"


/*
****************************************************************************************************
*       INTERNAL MACROS
****************************************************************************************************
*/

#define MAX_ASSIGNMENTS     (CC_MAX_DEVICES * CC_MAX_ACTUATORS * CC_MAX_ASSIGNMENTS)


/*
****************************************************************************************************
*       INTERNAL CONSTANTS
****************************************************************************************************
*/


/*
****************************************************************************************************
*       INTERNAL DATA TYPES
****************************************************************************************************
*/


/*
****************************************************************************************************
*       INTERNAL GLOBAL VARIABLES
****************************************************************************************************
*/

static cc_assignment_t g_assignments[MAX_ASSIGNMENTS];


/*
****************************************************************************************************
*       INTERNAL FUNCTIONS
****************************************************************************************************
*/


/*
****************************************************************************************************
*       GLOBAL FUNCTIONS
****************************************************************************************************
*/

cc_assignment_t *cc_assignment_new(void)
{
#ifdef XC16
    int i;
#endif
    static int assignments_initialized;
    if (!assignments_initialized)
    {
#ifdef XC16
        for (i = 0; i < MAX_ASSIGNMENTS; i++)
#else
        for (int i = 0; i < MAX_ASSIGNMENTS; i++)
#endif
            g_assignments[i].id = -1;

        assignments_initialized = 1;
    }
#ifdef XC16
    for (i = 0; i < MAX_ASSIGNMENTS; i++)
#else
    for (int i = 0; i < MAX_ASSIGNMENTS; i++)
#endif
    {
        cc_assignment_t *assignment = &g_assignments[i];

        if (assignment->id == -1)
        {
            assignment->id = 0;
            assignment->actuator_pair_id = -1;
            return assignment;
        }
    }

    return NULL;
}

int cc_assignment_delete(int assignment_id)
{
#ifdef XC16
    int i;
#endif
#ifdef XC16
    for (i = 0; i < MAX_ASSIGNMENTS; i++)
#else
    for (int i = 0; i < MAX_ASSIGNMENTS; i++)
#endif
    {
        cc_assignment_t *assignment = &g_assignments[i];

        if (assignment->id >= 0 && (assignment_id == assignment->id || assignment_id == -1))
        {
            cc_actuator_unmap(assignment);

            assignment->id = -1;

#ifdef CC_OPTIONS_LIST_SUPPORTED
            // free list memory
            options_list_destroy(assignment->list_items);
#endif

            if (assignment_id >= 0)
                return assignment->actuator_id;
        }
    }

    return -1;
}

cc_assignment_t *cc_assignment_get(int assignment_id)
{
    for (int i = 0; i < MAX_ASSIGNMENTS; i++)
    {
        cc_assignment_t *assignment = &g_assignments[i];

        if (assignment_id == assignment->id)
            return assignment;
    }

    return NULL;
}

inline void cc_assignments_clear(void)
{
    cc_assignment_delete(-1);
}
