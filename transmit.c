/**
 * Please see README.md and LICENSE
 *
 * Copyright 2020 oshablue / nisch s. except where otherwise noted
 */

#include <xc.h>
#include "transmit.h"


void setTxChannel(uint8_t newVal) {
    uint8_t portoutputval = txChanMap[newVal];
    TX_CTRL_PORT = portoutputval;
    g_currentTxChan = newVal;
}
