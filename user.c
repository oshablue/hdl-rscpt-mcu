/**
 * Please see README.md and LICENSE
 *
 * Copyright 2020 oshablue / nisch s. except where otherwise noted
 */


#include "user.h"
#include "mcc_generated_files/tmr0.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/adcc.h"


void stopContinuousPAQing(void) {

    // regardless of mode setting, let's always disable this for now, whenever
    // this function is called
    TMR0_StopTimer();   //
    TMR0_Initialize();  // Does this help?
}

void startContinuousPAQing(void) {
    TMR0_Initialize();
    TMR0_SetInterruptHandler( CONTINUOUS_PAQ_TMR_ISR );
    //TMR0_Reload();
    TMR0_StartTimer();
}

void restartContinuousPAQingIfNeeded(void) {
    // Let's not check the actual T0CON0bits.T0EN because again,
    // perhaps it is enabled only temporarily for a single PAQ etc.
    if ( g_modeIsContinuousPaq ) {
        startContinuousPAQing();
    }
}

void resetWaveformSequenceCount(void) {
    // RST implemented as active low
    // is to the hardware capture controller
    // and is RC3 output from this MCU to that controller.
    // Active low means it idles high, and to reset, set it low.

    IO_RC3_SetLow();
    IO_RC3_SetHigh();
}


void setRxDelay(uint8_t newVal) {

    //uint8_t b0 = newVal & 0x01;
    //uint8_t b1 = ( newVal & 0x02 ) >> 1;

    if ( newVal & 0x01 ) {
        RX_DELAY_CTRL_B0_SETHIGH(); // was read_mem_in
    } else {
        RX_DELAY_CTRL_B0_SETLOW();
    }

    if ( newVal & 0x02 ) {
        RX_DELAY_CTRL_B1_SETHIGH();  // was capt_done
    } else {
        RX_DELAY_CTRL_B1_SETLOW();
    }

}



/**
 *
 * No params at present - just the only ADC pin assigned, as a customer
 * request to squeeze this in to help solve a system integration issue
 *
 * Inputs will probably float high to 10-bit max output = 0b11 1111,1111 = 1023
 * Except note SPI input below, if used instead.
 *
 * Please see notes and caveats about this.  Some are here:
 * 1. Use caution.
 * 2. Implementing ADC on the HDL-0108-RSCPT hardware is re-using pins for ADC.
 * 3. So, typically, we are either re-using a programming pin, here PGC, for it.
 *    Or, we are using an SPI pin.
 *    Tested pins include: PGC, PGD, and MISO_GREEN (RC1) signals.  But, eg RC1
 *    is normally pulled up, by some SPI devices on the bus.  It was tested with
 *    a high impedance source driving to about 2.8 from the usual 3.3 and that
 *    worked ok.  But, beware and be cautious.  And if you use both, make sure
 *    to manage the toggling between configs appropriately.
 *    Also, using the PGC pin for example means that you probably can't run a
 *    debug session.  This wasn't much problem for our work flow, but could be
 *    for yours.  It does not however interfere with regular programming (dropping
 *    the hex code into memory.
 * 4. Since FVR can't be set (the ADC positive ref voltage) to anything above
 *    VDD and we are using VDD on the HDL-0108-RSCPT as 3.3VDC, you can't measure
 *    signals greater than 3.3VDC.  So if you need up to 5VDC for example, use an
 *    external divider.
 *
 *    ANB7 = PGD
 *    ANB6 = PGC
 *    ANC1 = "MOSI_GREEN" from the SPI
 */
adc_result_t getAndSendAdcSample(void) {
    adcc_channel_t mcc_gen_adcc_settings = channel_ANC1;
    adc_result_t result;
    //result = ADCC_GetSingleConversion(mcc_gen_adcc_settings);
    ADCC_Initialize();
    ADCC_DisableContinuousConversion();
    ADCC_DischargeSampleCapacitor();
    __delay_us(100);
    ADCC_StartConversion(mcc_gen_adcc_settings);
    __delay_us(100);
    while(!ADCC_IsConversionDone());
    result = ADCC_GetConversionResult();
    __delay_us(100);
    ADCC_StopConversion();


    return result;
}
