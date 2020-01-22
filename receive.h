/**
 * Please see comments in main.c or in a readme.md if included or at oshablue.com
 */
/* 
 * File:   receive.h
 * Author: nisch
 *
 * Created on December 2, 2019, 12:17 PM
 */

#ifndef RECEIVE_H
#define	RECEIVE_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>



#ifdef	__cplusplus
extern "C" {
#endif
    
    uint8_t g_currentRxChan = 0;

    #define AIMUX_ADDRESS_LAT       LATE    // RE0 - RE2 = A0 - A2
    #define AIMUX_ADDRESS_MASK      0x07    // 0000 0111 = 0 7 = 0x07
    #define AIMUX_ADDR_SHIFT_BITS   0       // we start at RE0
    #define AIMUX_CHANNEL_ON(B)         (AIMUX_ADDRESS_LAT = (AIMUX_ADDRESS_LAT & (~AIMUX_ADDRESS_MASK)) | (B<<AIMUX_ADDR_SHIFT_BITS))
    
    // Mapping of channels is 1:1 for Ch 1 - 4 and then 
    // in reverse order for Ch 5 - 8
    const uint8_t rxChanMap[8] = {
        0x00, 0x01, 0x02, 0x03,
        0x07, 0x06, 0x05, 0x04
    };

    void setRxChannel(uint8_t newVal); // Set Rx channel
    
    
    
    
    // Mapping of gain: Just bitwise using the port bits
    #define AIGAIN_ADDRESS_LAT       LATC    // RC4 - RC7 = A0 - A3
    #define AIGAIN_ADDRESS_MASK      0xF0    // 1111 0000 = F 0 = 0xF0
    #define AIGAIN_ADDR_SHIFT_BITS   4       // we start at RC4
    #define AIGAIN_SET(B)         (AIGAIN_ADDRESS_LAT = (AIGAIN_ADDRESS_LAT & (~AIGAIN_ADDRESS_MASK)) | (B<<AIGAIN_ADDR_SHIFT_BITS))
    // Enable is tied high via 100k in standard build to the VAA3.3 supply rail
    // So, although it can be controlled via RA0
    // it may not be needed for basic functions
    void setRxGain(uint8_t newVal);

    
    // Rx Delay on Capture Hardware - Control
    // Currently, default implementation is that each bit increment 
    // is additional 30usec of delay
    // However, could also implement as a mask for arbitrary receive delay
    // That could be an alternate "mode" for control of Rx delay
    //#define RX_DELAY_CTRL_B0    RA1
    //#define RX_DELAY_CTRL_B1    RB4
    //#define RX_DELAY_CTRL_B0_SETHIGH IO_RA1_SetHigh
    //#define RX_DELAY_CTRL_B0_SETLOW  IO_RA1_SetLow
    //#define RX_DELAY_CTRL_B1_SETHIGH IO_RB4_SetHigh
    //#define RX_DELAY_CTRL_B1_SETLOW  IO_RB4_SetLow
    void setRxDelay(uint8_t newVal);
    

#ifdef	__cplusplus
}
#endif

#endif	/* RECEIVE_H */

