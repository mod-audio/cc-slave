#include "control_chain.h"

#define NUM_ACTUATORS  8

int main(void)
{
    static float actuators[NUM_ACTUATORS];

    cc_init(0, 0);

    cc_device_t *device = cc_device_new("Test", "uri:TestDevice");
    for (int i = 0; i < NUM_ACTUATORS; i++)
    {
        char name[16] = {"Act #"};
        name[6] = '1' + i;
        name[7] = 0;

        cc_actuator_config_t actuator_config;
        actuator_config.type = CC_ACTUATOR_MOMENTARY;
        actuator_config.name = name;
        actuator_config.value = &actuators[i];
        actuator_config.min = 0.0;
        actuator_config.max = 1.0;
        actuator_config.supported_modes = CC_MODE_TOGGLE | CC_MODE_TRIGGER;
        actuator_config.max_assignments = 1;

        cc_actuator_t *actuator = cc_actuator_new(&actuator_config);
        cc_device_actuator_add(device, actuator);
    }

    return 0;
}
