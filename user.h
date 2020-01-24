/**
 * Please see README.md and LICENSE
 *
 * Copyright 2020 oshablue / nisch s. except where otherwise noted
 */

#ifndef USER_H
#define	USER_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <conio.h>
#include <string.h>

#include "transmit.h"
#include "receive.h"
#include "commands.h"

#include "mcc_generated_files/adcc.h"
#include "mcc_generated_files/mcc.h"

#ifdef	__cplusplus
extern "C" {
#endif

// TODO - make this configurable!
//#define SUPERDEBUG  1


#define OK  1
#define ERR 0

#define DEFAULT_DAC_OUTPUT_IDLE     0x00        // HIVDC output should be near zero
#define DEFAULT_DAC_OUTPUT_READY    0x0f        // HIVDC output should be near working range
#define DEFAULT_RX_CHANNEL          0x01        // Ch 1
#define DEFAULT_RX_GAIN             0x00        // 0x00 - 0x0f
#define DEFAULT_TX_CHANNEL          0x01        // Ch 1
#define DEFAULT_TX_SSR_TSD_STATE    SSR_TSD_ALL_OFF
#define ALL_SUBSYSTEMS_OFF          IO_RB0_SetLow
#define ALL_SUBSYSTEMS_ON           IO_RB0_SetHigh
#define HW_CAPT_DEVICE_NRST         IO_RC3_SetHigh

#define RX_DELAY_CTRL_B0_SETHIGH IO_RA1_SetHigh
#define RX_DELAY_CTRL_B0_SETLOW  IO_RA1_SetLow
#define RX_DELAY_CTRL_B1_SETHIGH IO_RB4_SetHigh
#define RX_DELAY_CTRL_B1_SETLOW  IO_RB4_SetLow

#define CONTINUOUS_PAQ_TMR_ISR      myTimer0ISR
#define UART_RX_WATCHDOG_TMR_ISR    mtTimer2ISR

#define NUM_RX_CHANNELS             8


    bool isContinuousPAQing(void);
    void stopContinuousPAQing(void);
    void startContinuousPAQing(void);
    void restartContinuousPAQingIfNeeded(void);
    void resetWaveformSequenceCount(void);
    adc_result_t getAndSendAdcSample(void); // Please see notes and caveats about this

    // Moved from main
    void myTimer0ISR(void);

    extern bool g_modeIsContinuousPaq;
    //extern bool g_wasContPAQ;


#ifdef	__cplusplus
}
#endif

#endif	/* USER_H */
