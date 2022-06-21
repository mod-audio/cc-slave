/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include "control_chain.h"
#include "msg.h"
#include "handshake.h"
#include "device.h"
#include "update.h"
#include "assignment.h"


/*
****************************************************************************************************
*       INTERNAL MACROS
****************************************************************************************************
*/

#define MSG_MAX_INSTANCES   2

// calculate how many bytes fit inside the frame
// FIXME: in the future change CC_BAUD_RATE_FALLBACK to CC_BAUD_RATE
#define BYTES_PER_FRAME     ((CC_FRAME_PERIOD * CC_BAUD_RATE_FALLBACK) / (1000000 * 10))

// maximum number of updates which fit inside the frame
// the update command has 6 bytes of overhead and each update data need 5 bytes
#define MAX_UPDATES_PER_FRAME   ((BYTES_PER_FRAME - 6) / 5)


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

cc_msg_t *cc_msg_new(uint8_t *buffer)
{
    static int count;
    static cc_msg_t msg_cache[MSG_MAX_INSTANCES];

    if (count >= MSG_MAX_INSTANCES)
        return 0;

    cc_msg_t *msg = &msg_cache[count++];

    // set buffers
    msg->header = buffer;
    msg->data = &buffer[CC_MSG_HEADER_SIZE];

    return msg;
}

int cc_msg_parser(const cc_msg_t *msg, void *data_struct)
{
    uint8_t *pdata = msg->data;

    if (msg->command == CC_CMD_HANDSHAKE)
    {
        cc_handshake_mod_t *handshake = data_struct;

        // random id
        handshake->random_id = *((uint16_t *) pdata);
        pdata += sizeof(uint16_t);

        // status, device id
        handshake->status = *pdata++;
        handshake->device_id = *pdata++;
    }
    if (msg->command == CC_CMD_DEV_CONTROL)
    {
        int *enable = data_struct;
        *enable = *pdata++;
    }
    else if (msg->command == CC_CMD_ASSIGNMENT)
    {
        cc_assignment_t *assignment = data_struct;

        // assignment id, actuator id
        assignment->id = *pdata++;
        assignment->actuator_id = *pdata++;

        // assignment label
#ifdef CC_STRING_NOT_SUPPORTED
        // skip label
        uint8_t str_size = *pdata++;
        pdata += str_size;
#else
        // read label
        pdata += str16_deserialize(pdata, &assignment->label);
#endif

        // value, min, max, def
        pdata += bytes_to_float(pdata, &assignment->value);
        pdata += bytes_to_float(pdata, &assignment->min);
        pdata += bytes_to_float(pdata, &assignment->max);
        pdata += bytes_to_float(pdata, &assignment->def);

        // mode
        uint8_t *pmode = (uint8_t *) &assignment->mode;
        *pmode++ = *pdata++;
        *pmode++ = *pdata++;
        *pmode++ = *pdata++;
        *pmode++ = *pdata++;

        // steps
        uint8_t *psteps = (uint8_t *) &assignment->steps;
        *psteps++ = *pdata++;
        *psteps++ = *pdata++;

        // unit
#ifdef CC_STRING_NOT_SUPPORTED
        // skip unit
        str_size = *pdata++;
        pdata += str_size;
#else
        // read unit
        pdata += str16_deserialize(pdata, &assignment->unit);
#endif
        // default value of list count
        assignment->list_count = 0;

#ifdef CC_OPTIONS_LIST_SUPPORTED
        // list count
        assignment->list_count = *pdata++;

        uint8_t list_items_in_frame = CC_OPTIONS_LIST_FRAME_SIZE;
        if (assignment->list_count < list_items_in_frame)
            list_items_in_frame = assignment->list_count;

        // list items
        assignment->list_items = options_list_create(list_items_in_frame);

        for (int i = 0; i < list_items_in_frame; i++)
        {
            option_t *item = assignment->list_items[i];

            pdata += str16_deserialize(pdata, &item->label);
            pdata += bytes_to_float(pdata, &item->value);
        }

        for (int i = 0; i < list_items_in_frame; i++)
        {
            if (assignment->value == assignment->list_items[i]->value)
            {
                assignment->list_index = i;
                break;
            }
        }
#endif
    }
    else if (msg->command == CC_CMD_UNASSIGNMENT)
    {
        uint8_t *assignment_id = data_struct;

        *assignment_id = *pdata++;
    }
    else if (msg->command == CC_CMD_SET_VALUE)
    {
        cc_set_value_t *update = data_struct;

        // assignment id, actuator id
        update->assignment_id = *pdata++;
        update->actuator_id = *pdata++;

        // value to set
        bytes_to_float(pdata, &update->value);
    }
#ifdef CC_OPTIONS_LIST_SUPPORTED
    else if (msg->command == CC_CMD_UPDATE_ENUMERATION)
    {
        // assignment id, actuator id
        uint8_t *assignment_id = data_struct;

        *assignment_id = *pdata++;
        uint8_t actuator_id = *pdata++;

        cc_assignment_t *assignment = cc_assignment_get(*assignment_id);
        uint8_t list_items_in_frame = CC_OPTIONS_LIST_FRAME_SIZE;
        if (assignment->list_count < list_items_in_frame)
            list_items_in_frame = assignment->list_count;

        uint8_t list_index = *pdata++;

        //free old list
        options_list_destroy(assignment->list_items);

        //create a new one
        assignment->list_items = options_list_create(list_items_in_frame);

        for (int i = 0; i < list_items_in_frame; i++)
        {
            option_t *item = assignment->list_items[i];

            pdata += str16_deserialize(pdata, &item->label);
            pdata += bytes_to_float(pdata, &item->value);
        }

        assignment->list_index = list_index;
    }
#endif
    return 0;
}

int cc_msg_builder(int command, const void *data_struct, cc_msg_t *msg)
{
    uint8_t *pdata = msg->data;
    msg->command = command;

    if (command == CC_CMD_HANDSHAKE)
    {
        const cc_handshake_t *handshake = data_struct;

        // random id
        uint8_t *pvalue = (uint8_t *) &handshake->random_id;
        *pdata++ = *pvalue++;
        *pdata++ = *pvalue++;

        // protocol version
        *pdata++ = handshake->protocol.major;
        *pdata++ = handshake->protocol.minor;

        // firmware version
        *pdata++ = handshake->firmware.major;
        *pdata++ = handshake->firmware.minor;
        *pdata++ = handshake->firmware.micro;
    }
    else if (command == CC_CMD_DEV_DESCRIPTOR)
    {
        const cc_device_t *device = data_struct;

        // serialize uri
        pdata += cstr_serialize(&device->uri, pdata);

        // serialize label
        pdata += cstr_serialize(&device->label, pdata);

        // number of actuators
        *pdata++ = device->actuators_count;

        // serialize actuators data
        for (unsigned int i = 0; i < device->actuators_count; i++)
        {
            cc_actuator_t *actuator = device->actuators[i];

            // actuator name
            pdata += str16_serialize(&actuator->name, pdata);

            // supported modes
            uint8_t *modes = (uint8_t *) &actuator->supported_modes;
            *pdata++ = *modes++;
            *pdata++ = *modes++;
            *pdata++ = *modes++;
            *pdata++ = *modes++;

            // max assignments
            *pdata++ = actuator->max_assignments;
        }

        //number of actuatorgroups
        *pdata++ = device->actuatorgroups_count;

        //serialize actuatorgroups data
        for (unsigned int i = 0; i < device->actuatorgroups_count; i++)
        {
            cc_actuatorgroup_t *actuatorgroup = device->actuatorgroups[i];

            // actuatorgroup name
            pdata += str16_serialize(&actuatorgroup->name, pdata);

            // actuators in actuatorgroup
            *pdata++ = actuatorgroup->actuators_in_group[0];
            *pdata++ = actuatorgroup->actuators_in_group[1];
        }

        //amount of enumeration items we recieve at once
        *pdata++ = CC_OPTIONS_LIST_FRAME_SIZE;

        //amount of actuator pages
        *pdata++ = device->actuator_pages;

        //CC chain ID
        *pdata++ = device->chain_id;
    }
    else if (command == CC_CMD_ASSIGNMENT || command == CC_CMD_UNASSIGNMENT || command == CC_CMD_SET_VALUE)
    {
        // no data
    }
    else if (command == CC_CMD_DATA_UPDATE)
    {
        int count = cc_updates_count();
        if (count > MAX_UPDATES_PER_FRAME)
            count = MAX_UPDATES_PER_FRAME;

        *pdata++ = count;

        // serialize updates data
        while (count--)
        {
            cc_update_t update;
            cc_update_pop(&update);

            uint8_t *pvalue = (uint8_t *) &update.value;
            *pdata++ = update.assignment_id;
            *pdata++ = *pvalue++;
            *pdata++ = *pvalue++;
            *pdata++ = *pvalue++;
            *pdata++ = *pvalue++;
        }
    }
    else if (msg->command == CC_CMD_REQUEST_CONTROL_PAGE)
    {
        const int *page = data_struct;

        *pdata++ = *page;
    }
    else
    {
        return -1;
    }

    msg->data_size = (pdata - msg->data);

    return 0;
}
