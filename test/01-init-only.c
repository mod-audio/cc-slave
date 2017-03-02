#include <stdio.h>
#include "control_chain.h"

int main(void)
{
    cc_init(0, 0);
    printf("protocol version: %s\n", CC_PROTOCOL_VERSION);
    printf("firmware version: %s\n", CC_FIRMWARE_VERSION);

    return 0;
}
