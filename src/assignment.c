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

#define MAX_ASSIGNMENTS     (CC_MAX_DEVICES * CC_MAX_ACTUATORS * CC_MAX_ASSIGNMENTS * MAX_PAGES)


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
    static int assignments_initialized;
    if (!assignments_initialized)
    {
        for (int i = 0; i < MAX_ASSIGNMENTS; i++)
            g_assignments[i].id = -1;

        assignments_initialized = 1;
    }

    for (int i = 0; i < MAX_ASSIGNMENTS; i++)
    {
        cc_assignment_t *assignment = &g_assignments[i];

        if (assignment->id == -1)
        {
            return assignment;
        }
    }

    return NULL;
}

int cc_assignment_delete(int assignment_id)
{
    for (int i = 0; i < MAX_ASSIGNMENTS; i++)
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

#ifdef CC_OPTIONS_LIST_SUPPORTED
cc_assignment_t *cc_assignment_update_enumeration(cc_update_enumeration_t *update)
{
    //first free old list memory
    cc_assignment_t *assignment = cc_assignment_get(update->assignment_id);

    uint8_t list_items_in_frame = CC_OPTIONS_LIST_FRAME_SIZE;
    if (assignment->list_count < list_items_in_frame)
        list_items_in_frame = assignment->list_count;

    for (int i = 0; i < list_items_in_frame; i++)
    {
        assignment->list_items[i]->label.size = update->list_items[i]->label.size;
        assignment->list_items[i]->value = update->list_items[i]->value;
        strcpy(assignment->list_items[i]->label.text, update->list_items[i]->label.text);
    }

    //set the indexes
    assignment->list_index = update->list_index;

    return assignment;
}
#endif

inline void cc_assignments_clear(void)
{
    cc_assignment_delete(-1);
}
