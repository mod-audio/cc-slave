#ifndef CONTROL_CHAIN_H
#define CONTROL_CHAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include <stdint.h>
#include "device.h"
#include "handshake.h"
#include "actuator.h"
#include "update.h"
#include "msg.h"
#include "config.h"


/*
****************************************************************************************************
*       MACROS
****************************************************************************************************
*/

// protocol version
#define CC_PROTOCOL_MAJOR       0
#define CC_PROTOCOL_MINOR       1

// define serial communication baud rate
#define CC_BAUD_RATE       115200

// define versions as strings
#define CC_PROTOCOL_VERSION VERSION2_STR(CC_PROTOCOL_MAJOR, CC_PROTOCOL_MINOR)
#define CC_FIRMWARE_VERSION VERSION3_STR(CC_FIRMWARE_MAJOR, CC_FIRMWARE_MINOR, CC_FIRMWARE_MICRO)


/*
****************************************************************************************************
*       CONFIGURATION
****************************************************************************************************
*/

#ifndef CC_USE_EXTERNAL_CONFIG
// maximum number of devices that can be created
#define CC_MAX_DEVICES      1
// maximum number of actuators that can be created per device
#define CC_MAX_ACTUATORS    1
// maximum number of assignments that can be created per actuator
#define CC_MAX_ASSIGNMENTS  1

// define firmware version
#define CC_FIRMWARE_MAJOR   0
#define CC_FIRMWARE_MINOR   0
#define CC_FIRMWARE_MICRO   0
#endif


/*
****************************************************************************************************
*       DATA TYPES
****************************************************************************************************
*/

typedef struct cc_data_t {
    uint8_t *data;
    uint32_t size;
} cc_data_t;

typedef struct cc_event_t {
    int id;
    void *data;
} cc_event_t;

enum {CC_EV_HANDSHAKE_FAILED, CC_EV_ASSIGNMENT, CC_EV_UNASSIGNMENT, CC_EV_UPDATE,
      CC_EV_DEVICE_DISABLED, CC_EV_MASTER_RESETED};


/*
****************************************************************************************************
*       FUNCTION PROTOTYPES
****************************************************************************************************
*/

void cc_init(void (*response_cb)(void *arg), void (*events_cb)(void *arg));
void cc_process(void);
void cc_parse(const cc_data_t *received);


/*
****************************************************************************************************
*       CONFIGURATION ERRORS
****************************************************************************************************
*/

#if !defined(CC_FIRMWARE_MAJOR) && !defined(CC_FIRMWARE_MINOR) && !defined(CC_FIRMWARE_MICRO)
#error "The macros CC_FIRMWARE_MAJOR, CC_FIRMWARE_MINOR and CC_FIRMWARE_MICRO must to be defined"
#endif


#ifdef __cplusplus
}
#endif

#endif
