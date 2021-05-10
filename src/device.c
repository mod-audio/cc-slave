/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include "control_chain.h"
#include "device.h"


/*
****************************************************************************************************
*       INTERNAL MACROS
****************************************************************************************************
*/


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

static cc_device_t g_devices[CC_MAX_DEVICES];
static unsigned int g_devices_count;
static cc_actuator_t *g_actuators[CC_MAX_DEVICES*CC_MAX_ACTUATORS];
static cc_actuatorgroup_t *g_actuatorgroups[CC_MAX_ACTUATORGROUPS];

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

cc_device_t *cc_device_new(const char *name, const char *uri)
{
    if (g_devices_count >= CC_MAX_DEVICES)
        return 0;

    cc_device_t *device = &g_devices[g_devices_count];

    // create device URI and label
    cstr_create(uri, &device->uri);
    cstr_create(name, &device->label);

    // create a list of actuators
    device->actuators = &g_actuators[g_devices_count * CC_MAX_ACTUATORS];
    device->actuators_count = 0;

    // set actuatrogroupscount
    device->actuatorgroups = &g_actuatorgroups[g_devices_count * CC_MAX_ACTUATORGROUPS];
    device->actuatorgroups_count = 0;

    g_devices_count++;

    return device;
}

void cc_device_actuator_add(cc_device_t *device, cc_actuator_t *actuator)
{
    if (device->actuators_count >= CC_MAX_ACTUATORS)
        return;

    device->actuators[device->actuators_count] = actuator;
    device->actuators_count++;
}

void cc_device_actuatorgroup_add(cc_device_t *device, cc_actuatorgroup_t *actuatorgroup)
{
    if (device->actuatorgroups_count >= CC_MAX_ACTUATORGROUPS)
        return;

    device->actuatorgroups[device->actuatorgroups_count] = actuatorgroup;
    device->actuatorgroups_count++;
}

cc_actuator_t *cc_device_actuator_get(cc_device_t *device, uint8_t actuator_id)
{
    if (actuator_id < device->actuators_count) {
        return device->actuators[actuator_id];
    }
    return 0;
}

cc_device_t *cc_device_get(void)
{
    return &g_devices[0];
}
