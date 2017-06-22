/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include <stdlib.h>
#include "control_chain.h"
#include "handshake.h"


/*
****************************************************************************************************
*       INTERNAL MACROS
****************************************************************************************************
*/

// step between generated random numbers in bytes
#define RANDOM_STEP_BYTES   3

// random generation boundaries
#define RANDOM_MAX      5000
#define RANDOM_MIN      100

// convert step value from bytes to microseconds
#define RANDOM_STEP     ((10 * 1000000 * RANDOM_STEP_BYTES)  / CC_BAUD_RATE)

// macro to generate random number within a range
#define RANDOM_RANGE(min, max)              ((rand() % max) + min)

// macro to generate random number within a range and using a defined step
#define RANDOM_RANGE_STEP(min, max, step)   (RANDOM_RANGE(min/step, max/step) * step)


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

static cc_handshake_t g_handshake;


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

cc_handshake_t *cc_handshake_generate(cstr_t *uri)
{
    cc_handshake_t *handshake = &g_handshake;

    // uri
    handshake->uri = uri;

    // generate random number
    handshake->random_id = RANDOM_RANGE_STEP(RANDOM_MIN, RANDOM_MAX, RANDOM_STEP);

    // protocol version
    handshake->protocol.major = CC_PROTOCOL_MAJOR;
    handshake->protocol.minor = CC_PROTOCOL_MINOR;
    handshake->protocol.micro = 0;

    // firmware version
    handshake->firmware.major = CC_FIRMWARE_MAJOR;
    handshake->firmware.minor = CC_FIRMWARE_MINOR;
    handshake->firmware.micro = CC_FIRMWARE_MICRO;

    return handshake;
}
