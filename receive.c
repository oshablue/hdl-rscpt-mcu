/**
 * Please see README.md and LICENSE
 *
 * Copyright 2020 oshablue / nisch s. except where otherwise noted
 */

#include <xc.h>
#include "receive.h"

void setRxChannel(uint8_t newValBaseOne) {

    // TODOLT with code optimization and refinement and more program room,
    // update for sanity checking the bounds of the input value

    // If zero is passed, exit
    if ( newValBaseOne == 0) {
        return;
    }

    g_currentRxChan = newValBaseOne;

    // otherwise, process and set the channel

    newValBaseOne -= 1; // using base 1 here, so Ch 1 incoming value => chanMap[0]
    // because with enable always on, there are just 8 values in the array
    // i.e. no "off"

    uint8_t rxChOutputVal = rxChanMap[newValBaseOne];

    AIMUX_CHANNEL_ON(rxChOutputVal);


}

void setRxGain(uint8_t newVal) {
    newVal = newVal > 0x0F ? 0x0F : newVal;
    AIGAIN_SET(newVal);
}


// setRxDelay is currently placed in user.c/h due to header inclusion
// simplification
