/**
 * Please see comments in main.c or in a readme.md if included or at oshablue.com
 */
/* 
 * File:   transmit.h
 * Author: nisch
 *
 * Created on December 2, 2019, 12:15 PM
 */

#ifndef TRANSMIT_H
#define	TRANSMIT_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>



#ifdef	__cplusplus
extern "C" {
#endif
    
#define TX_CTRL_PORT    LATD            // this should bea LAT port
    
    // Tx SSR Channel Selection
    // Port D: All outputs to select individual channels on SSRs
    // RD7 => TXSSRCHAN1 ... RD0 => TXSSRCHAN8
    // txChanReal[base_zero_desired_chan] = real world pin #
    // Since we will use just PortDbits really by writing to the LATD
    // Bit: 7    6    5    4      3    2    1    0
    // Ch:  2    1    4    3      6    5    8    7
    // b:  1000 0100 0010 0001   etc.
    // 0x: 0x80 0x40 0x20 0x10   0x08 0x04 0x02 0x01 (except swap the pairs, as below)
    // All if chan = 0, nothing selected
    // Where output real chan mapping [input selection chan]
    const uint8_t txChanMap[9] = {
        0x00, 0x40, 0x80, 0x10, 0x20, 0x04, 0x08, 0x01, 0x02
    };
    
    
    // Tx Error (Init as Input to read high Z with internal pullup or 0 state)
    // TSD 1 - 4 => Port A 3 - 6
    // Could also drive this with open collector/drain - see datasheet
    // If driven to zero, suspends function/output (opens all switches) of SSR
    // All off means use MCU output to drive SSR TSD input to zero
    // Error (thermal shutdown) means MCU is input and SSR TSD are outputs
    
    uint8_t g_currentTxChan = 0;
    
    

    void setTxChannel(uint8_t newCh); // Set Tx channel
    
    // Used to blank/turn off the channel on the SSR to abate noise that otherwise
    // gets overlaid onto the Rx channel during sensitive receive time unfortunately
    // This appears to be necessary after cycling SSR TSD signal(s)
    void cycleTxChannelOffAndOn(void);  // Set chan to zero and back to selected value
    
    
    
    
    #define SSR_TSD_ADDRESS_LAT       LATA    // RA3 - RA6 = TSD1 - TSD4
    #define SSR_TSD_ADDRESS_MASK      0x78    // 0111 1000 = 7 8 = 0x78
    #define SSR_TSD_ADDR_SHIFT_BITS   3       // we start at RA3
    #define SSR_TSD_SET(B)         (SSR_TSD_ADDRESS_LAT = (SSR_TSD_ADDRESS_LAT & (~SSR_TSD_ADDRESS_MASK)) | (B<<SSR_TSD_ADDR_SHIFT_BITS))
    #define SSR_TSD_ALL_HIGH          SSR_TSD_SET(0x0F)
    #define SSR_TSD_ALL_LOW           SSR_TSD_SET(0x00)
    #define SSR_TSD_ALL_ON            SSR_TSD_ALL_HIGH 
    #define SSR_TSD_ALL_OFF           SSR_TSD_ALL_LOW


#ifdef	__cplusplus
}
#endif

#endif	/* TRANSMIT_H */

