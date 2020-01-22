/**
 * Please see comments in main.c or in a readme.md if included or at oshablue.com
 */

#include <xc.h>
#include "transmit.h"


void setTxChannel(uint8_t newVal) {
    uint8_t portoutputval = txChanMap[newVal];
    TX_CTRL_PORT = portoutputval;
    g_currentTxChan = newVal;
}
