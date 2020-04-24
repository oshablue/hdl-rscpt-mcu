/**

  Company:
 *  OshaBlue LLC / nisch / oshablue.com
 *  from template originally generated by:
 *  Microchip Technology Inc.

  File Name:
    main.c

  Summary:
 *  This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs.
 *  It has been updated to implement the basic functionality for the OshaBlue LLC 
 *  Ultrasonic pulse-and-acquire (PAQ) 8-channel rapid scan hardware implementation.
 * 
 *  There are many ways to run the hardware, many options for implementing state 
 *  machines, timing and control queues etc in firmware.  
 *  The code here, as is, is just one way, by example.
 *  This was developed rapidly to prove basic functionality of the hardware and 
 *  to demo basic methods for implementing control and data.
 * 
 *  There are probably a lot of ways to clean up this code!  For sure.  Go for it.
 *  As a working software package, there are transitions in code division and 
 *  convention, so you might see inconsistencies.  Customize and update as you 
 *  you need.
 * 
 *  LINTing would probably be good, and probably some DRYing.  Only has been 
 *  built with standard (zero?) optimizations on free compiler.  Runs though.
 *  May still be some hardware stack overflow warnings.  
 * 
 *  There remain many notes from the dev process, for reference, although much 
 *  has been cut and exists only in older commits.

  Description:
 *  See the oshablue.com website and web content for details regarding the 
 *  hardware platform that this firmware is intended to run.
 * 
    From the original generation of this template file:
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.78
        Device            :  PIC16F18875
        Driver Version    :  2.00
*/

/*
   
 * Code customization and hardware-specific implementation:
 * (c) 2019 OshaBlue LL
 * Please see license file.
 * 
 * Templates and driver-generated code, including code generated by MCC: 
 * (c) 2019 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 * 
 *  Regarding: Customizations and hardware-specific implementations by OshaBlue
 *  LLC / nisch:
 * 
 *  THIS SOFTWARE, INCLUDING ALL RELATED FILES IN THIS SOFTWARE PACKAGE,
 *  IS SUPPLIED BY OSHABLUE LLC "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL OSHABLUE BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, OSHABLUE'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO OSHABLUE FOR THIS 
    SOFTWARE.
*/

// Microchip MCC generated driver files:
#include <pic16f18875.h>

#include "mcc_generated_files/mcc.h"

// Declarations for custom user code
#include "user.h"


void myTimer2ISR(void);
void setDacOutput(uint8_t newVal);
void send_string(const char * x);
void myUartTxTester(void);
void rxDataHandler(void);     
uint8_t rxDataParseAndAccum(void);
void resetRxDataParseAndAccum(void);
void processControlQueue(void);
void clearRxPacket(void);
//void printRxPacket(void);
void processRxPacket(void);
void startUartRxWatchdog(void);
void clearUartRxWatchdog(void);
int8_t processTxCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2);
int8_t processRxCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2);
int8_t processPaqCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2);
int8_t processAltCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2);
void setPeripheralPaqFunctionsToIdle(void);
//void testInitPeripherals(void);


// For Rx receive state machine and supporting items:
// TODOLT could place into its own module eg
#define PACKET_MAX_SIZE         64
#define DEBUG_RX_PACKET_OUTPUT_SIZE 15
#define END_OF_PACKET_MARKER    0x50
#define START_OF_PACKET_MARKER  0x53
typedef enum {receivingDataNotStarted, receivingDataBuildingPacket, packetOverflow, invalidPacket, packetComplete} packetRXStates_t;
const char * debugRxStates[5] = { "rcvDatNotSt", "bldgPkt", "oflow", "inval", "pktComp" };
uint8_t rxPacket[PACKET_MAX_SIZE];
uint8_t packetPositionIndex = 0;
packetRXStates_t currState = receivingDataNotStarted;
// End of For Rx receive state machine and supporting items


uint8_t rxGain = 0;

// TODOLT could move to state machine for general control loop as well 
bool g_modeIsContinuousPaq = false;
bool g_modeIsChannelScan = false;
extern uint8_t g_currentRxChan;
extern uint8_t g_currentTxChan;


/*
                         Main application
 */
void main(void)
{
    
    // initialize the device - runs the MCC-generated driver init code
    SYSTEM_Initialize();
    
    // Please see notes and caveats about using ADC 
    ANSELCbits.ANSC1 = 1;
    TRISCbits.TRISC1 = INPUT;
    ADCC_DisableContinuousConversion();

    // TMR0 is full speed PAQ
    TMR0_SetInterruptHandler (myTimer0ISR);
    // TMR2 is UART RX timeout/watchdog
    TMR2_SetInterruptHandler (myTimer2ISR);
    
    
    // Use MCC to start with timer disabled / not running
    // So we don't need to do the following:
    //TMR2_StopTimer();

    // EUSART
    // Actually, yes the ISR is there, but runs behind the scenes.
    // We use a polling method in a processing queue or main capture ISR 
    // to actually deal with Rx 
    // data as that is sufficiently real-time in our use case.
    //EUSART_SetRxInterruptHandler (myUartRxISR);
    
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    // Set channels, DAQ output, etc. to idle state, though powered up and ready
    setPeripheralPaqFunctionsToIdle();
    
    
    while (1)
    {
        // TODOLT this could be put in an interrupt for guaranteed timing etc.
        // etc etc there are other control loop scenarios, incl state machines
        processControlQueue(); 
        
    }
    
}

/**
 * Sets the following: 
 * - continuous Paq mode = false 
 * - DAC output feeding the HIVDC module to zero
 * - Enable VCC and All Master enables, turning on the subsystems
 * - Set SSR TSDs to zero, disabling Tx channel output
 * - Tx channel: set to 1
 * - Rx channel: set to 1
 * - Rx gain: set to zero
 */
void setPeripheralPaqFunctionsToIdle() {
    
    g_modeIsContinuousPaq = false;
    
    setDacOutput((uint8_t)DEFAULT_DAC_OUTPUT_IDLE);
    
    ALL_SUBSYSTEMS_ON();
    
    DEFAULT_TX_SSR_TSD_STATE;  
    
    setTxChannel((uint8_t)DEFAULT_TX_CHANNEL);
    
    setRxChannel((uint8_t)DEFAULT_RX_CHANNEL);

    setRxGain((uint8_t)DEFAULT_RX_GAIN);

    setRxDelay((uint8_t)0);
    
    resetWaveformSequenceCount();
    
}




void startUartRxWatchdog() {
    
    // Tricky thing is that there is a callback counter within the
    // Timer2 ISR that is static volatile and only called from within the 
    // ISR - so would need to reset that too ... hmmm .. try Initialize?
    // Or set up so that only one ISR is needed to fire the callback
    // Thus set up the secondary prescaler too ...
    // Ok yes, set up primary and secondary for longest
    // So we have a 32ms interval for a single tick that calls the callback
    // Thus, a reset of the timer register is all that is needed.
    //TMR2_WriteTimer(0x00); // reset to 0x00 for this type of Timer
    
    // Let's go for initialize actually, and set the ISR, being cautious here in 
    // dev.  There may be a simpler method to do this.
    TMR2_Initialize(); 
    TMR2_SetInterruptHandler(  myTimer2ISR );
    TMR2_Start();
}



/** 
 * This function should always be called with the checkAndRestart if needed some time later ...
 */
void clearUartRxWatchdog() {
    TMR2_Stop();
    TMR2_WriteTimer(0x00); // proactive - is this needed?

}



void myTimer2ISR(void) {
    
    // Only fire once and then await new restart of timer on UART Rx
    TMR2_StopTimer();       
    
    // This is prob a good idea (the printf below about timeout)
    // but, compilermay make extra copies of the whole printf chain required,
    // since this occurs within an ISR - so it's a huge duplication of compiled 
    // code (probably?) - so better to set a flag and read it outside of the ISR 
    // https://www.microchip.com/forums/m1102166.aspx
    //printf("[ERR] RX T/O");
    // Also - commenting this out reduced stack depth warning from 27 to 21
    
    // Debug:
    //printRxPacket();

    // Reset the Rx packet and/or state?
    resetRxDataParseAndAccum();
    
    // Yeah, let's say timeout during cont paq - would want to go back to that 
    // state ...
    restartContinuousPAQingIfNeeded();
    
}


/**
 * Main capture ISR based on 128 captures/sec
 */
void myTimer0ISR(void) {
    
    // Work around for CapChip timing implementation
    // Testing reset to high always - only matters if was previously 
    // set to Low for active low reset for sequence count reset
    IO_RC3_SetHigh();
    
    // TODOLT MINOR/FUTURE could track Tx (pulse) channel and thus
    // only toggle the TSD signal for that particular corresponding
    // channel
    
    SSR_TSD_ALL_ON;
    
    
    
    // TODOLT - function for checking TSDs as inputs to look for fault condition
    // in the SSRs, when needed - requires the config and polling of values on 
    // these pins
    
    // Testing if necessary to cycle the channel back on after SSR TSD all back on
    // to re-latch the selected channel.
    // Turns out that no, it is not necessary.
    // The only critical piece to functional SSR blanking (TSD Off/On)
    // seems to be the delay_ms as noted in the next section
    //cycleTxChannelOffAndOn();
    
    // Advance Rx channel if necessary:
    // This is set up to always keep as consistent as possible 
    // the timing between ISR and pulse control -- hence the construction as thus 
    // Thus, we always call this, even if the channel has not changed
    setRxChannel(g_currentRxChan);
    setTxChannel(g_currentTxChan);
    
    // What happens, when capturing just an AWG waveform, about 
    // 3/4 of the way through, when something changes in the control signals 
    // and then the waveform capture changes - it is much like either some 
    // impedance thing or like some change in the channel selection on the AIMUX
    // Yes, it turns out, that for example, say with Tx DAC output voltage set to 
    // zero, so as not to pulse the AWG, when acquiring say a 1MHz 200mVpp signal,
    // on Ch 2, when the Tx channel is also set to 2, and thus the pulse control, 
    // and the SSR (esp TSD) signals are active on the channel, there is indeed:
    // easy-to-see in captured data waveform the amplitude of the signal starts 
    // lower and then increases in a near step function toward the end of the capture.
    // This is, at this writing anyway, as a best guess, the result of the turn-off 
    // delay when the SSR TSD all-off signal is sent.  Prior to the actual 
    // deactivation of the channel, there is a little more load on the Rx, thus
    // lower amplitude.  And then when the channels are truly off (SSR TSD ALL OFF)
    // there is a little bump up in the Rx signal amplitude.
    
    // The only the reason the SSR TSD signal is used to turn off the channel 
    // right after pulsing is to minimize noise on the more sensitive portion 
    // of the return Rx signal from transducers.
    // Without doing so, at high Rx gains, there are some apparent noisy spikes 
    // in the Rx waveform.
    // The trade-off is that then there is the gain (or load) change on the Rx signal.
    // This can be addressed in future hardware revs, now that we know these 
    // effects are present.
    // The amplitude (load) change inherent in the SSR TSD control during this ISR
    // for the Rx waveform can be shifted in time, by increasing the duration 
    // of the __delay_us() below, with the tradeoff there being that noise 
    // is present further into the waveform beginning, especially noticeable at 
    // high Rx gains.
    // This relates to the off-state transition time in the datasheet, but this 
    // was not specified for the TSD condition - nor was the noise issue anticipated.
    
    
    // Yeah post change switch/SSR-TSD etc -delay seems to matter
    // None, and there is no pulse through the SSR at all
    // Only 1 and the actual pulse on the XD gets long in time with poor RTZ
    // 2ms is tighter, 3ms even better, 4ms slightly even better than 3ms
    // Any/all of these could be ok - doing overhead, then switching the capture line
    // Also corresponds to max on-time in datasheet for the SSR
    // Ok so delay good for testing dev timing - but kill Rx responsiveness and buffer
    // in prod ... especially at 57.6kbps
    //__delay_ms(4); 
    // So implement with either add'l instructions or plain ol' delay loop with Rx
    // check, to avoid nested ISRs etc.
    uint16_t delCnt = 0x0000; // 0xFFFF even if exact 32MHz looping => 2ms
    // 0x03ff gives about 3ms delay from TSD goes high to real neg pulse starts
    // 4 ms using _delay_ms() function originally was our good target range
    // of course, adding any other logic to the while below will require a new 
    // timing measurement/calibration
    // For now, settling on the 0x03ff count timer delay
    while ( delCnt++ < 0x03ff ) {
        if ( PIR3bits.RCIF == 1 ) {
            return;
        }
    }
    

    // Will this work with timing of the clock?
    // Trying for full rate on the just the 0x1 Isr for the Tmr0
    // Yes, this works for the capture signal, just one cycle, no hold needed
    IO_RB5_SetHigh();
    IO_RB5_SetLow();
    // TODO put IO_RB5 in MACRO in user.h for example
    
    
    // Original note, as expanded upon above:
    // SSRs are noisy!  Or related ...
    // So, maybe if we deselect the channel or send TSR low to essentially turn 
    // off the channels also, we might stop the noise on the receive
    // Timing matters -
    // The external hardware handles the pulse timing
    // So we need the channel on long enough to pulse
    // and then off long enough and in time to get a cleaner Rx signal
    // Now, usually, or previously, these pins, for TSRs, were set to input
    // to read any overheat/fault status (ie normally high, may go low)
    // but now we initialize them as outputs and then send them low to turn off
    // chans ... hopefully, and hopefully killing that noise on the Rx line(s)
    //  
    // Testing with single TSR toggle to test the idea
    // NOTE: SSR mfg datasheets suggests using open-drain or open-collector
    // output (from MCU) to drive the TSD to zero, to prevent voltage mismatch
    // issues.  However, setting OD to 1 on each TSD pin, we weren't seeing
    // the TSD signal change on tests like this one, allowing TSD high to pulse
    // and then driving to zero to turn off the device (channel), so we are leaving
    // this is OD in MCC unchecked, and thus standard push-pull drive.  Since both
    // SSR and MCU VDDs are 3.3, this should be ok here and not create voltage 
    // mismatch/damage.
    // TODOLT MINOR/FUTURE: Investigate the above - including any MCC bug or other
    // characs, especially if rail voltages change.
    
    // Pulse control notes: early: cont'd:
    // At some point, the delay below appeared to have some impact on the pulse 
    // shaping, however at the latest tests, nearing demo draft release for 
    // end-user experimentation, the delay below has no impact on pulse shaping.
    // It does however have impact on the noise in the Rx signal and the time shift
    // of the amplitude (load) change for the Rx side (see above).
    // Leaving it here as a placeholder.
    __delay_us(1);
    
    // Now blank (turn off) the SSRs to minimize noise into the Rx waveform
    SSR_TSD_ALL_OFF;
    
    
    // Now any variable timing functionality at the end of the ISR
    // because it doesn't relate to pulse control
    if( g_modeIsChannelScan ) {
        g_currentRxChan++;
        g_currentTxChan++;
        if ( g_currentRxChan > NUM_RX_CHANNELS ) {
            g_currentRxChan = 1;
            g_currentTxChan = 1;
            //resetWaveformSequenceCount(); // Cleaner without this imlementation
            // Cleaner with this implementation:
            IO_RC3_SetLow();
        }
    }
    
    
}




void setDacOutput(uint8_t newVal) {
    // From MCC config at this time, initial value is (and should be) 
    // 0 (0 VDC)
    //
    // newVal is the 5-bit (2^5) value to set in the register
    // where 0 = 0 Volts output, ideally
    // where 32 = 2^5 = max output, around VDD = 3.3VDC, ideally
    // Output value in reality is approx:
    // VDD => 3.3VDC * (newVal) / (2^5)
    // So slightly less than 1 VDC = a value of 9 (out of 32 max)
    // So slightly more than 1 VDC = a value of 10 (out of 32 max)
    // As of demo customization release, 0x0f is a good starting value.
    // There is longer ramp-up time of the HIVDC output when a lower 
    // control voltage is used, even if you then need to trim back the HIVDC 
    // output.  It can be as long as 30 seconds or so.  Just due to the hardware 
    // implementation.  There are a lot of config options in hardware too.
    DAC_SetOutput(newVal);
}



void processControlQueue() {
    
    rxDataHandler();
    
}

// See:
// https://www.microchip.com/forums/m979514.aspx
// rxDataHandler (collect data) will unload the receive buffer until the buffer is empty
// or a end of packet marker is received and the packet is good.
// The function will return the byte count of the packet when it is finished,
// zero if the function is still collecting bytes,
// and a negative value if an error has occured.
/* --- copy the following code into a .dot file and run it through graphviz to see the state machine ---
*/



void clearRxPacket() {
    // TODOLT: Debug implementation of using memset here
    // likely something with string, or term \0 etc.
    uint8_t i = 0;
    for ( i = 0; i < sizeof(rxPacket); i++ ) {
        rxPacket[i] = 0;
    }
}



/*void printRxPacket() {
    printf("\r\n[%d max/sizeof rxPkt]\r\n", sizeof(rxPacket));
    printf("%s", "[rxPkt: ");
    uint8_t i = 0;
    for ( i = 0; i < DEBUG_RX_PACKET_OUTPUT_SIZE; i++) {
        printf("0x%02x ", rxPacket[i]);
    }
    printf("%s", "]\r\n\r\n");
}*/



void rxDataHandler() {
    
    switch(rxDataParseAndAccum()) {
        case receivingDataNotStarted:
            break;
        case receivingDataBuildingPacket:
            stopContinuousPAQing();
            startUartRxWatchdog();
            break;
        case packetOverflow:
            printf("%s", c_tx_packet_overflow);
            // Now either await packet Rx timeout or send P term char 
            // to advance to invalid state and then P again to complete the 
            // packet and move on
            break;
        case invalidPacket:
            printf("%s", c_tx_packet_invalid);
            // Now either await packet Rx timeout or send P term char 
            // to complete the packet and move on
            break;
        case packetComplete:
#ifdef SUPERDEBUG
            printRxPacket();
#endif 
            clearUartRxWatchdog();
            processRxPacket();
            clearRxPacket();
            restartContinuousPAQingIfNeeded();
#ifdef SUPERDEBUG
            printf("%s", "\r\nCleared rxPacket\r\n");
            printRxPacket();
#endif
        default:
            // do nothing
            break;
    }
    
}



void resetRxDataParseAndAccum() {
    // Since we can't write into the RX register several stop chars to 
    // reset a broken receive sequence, let's just reset the state
    // So, we need to update and allow some helpers
    // true = yes, reset statics or internally used globals
    currState = receivingDataNotStarted;
    packetPositionIndex = 0;
    clearRxPacket();
    
    
    // Or we could use global vars ...
    // Or probably better off using an object or class type of implementation
    // for this ultimately, like a state module instead 
}



uint8_t rxDataParseAndAccum(){
    
    packetRXStates_t nextState = receivingDataNotStarted;
    uint8_t c = 0x00;
    uint8_t retVal= 0x00;
    
    // TODO - whiles like this are scary to me generally.
    while ( EUSART_DataReady ) {
        
        c = EUSART_Read();
        
        
        
        // 
        // BTW non-term P or 0x50 equivs should be escaped
        // 0x4F ('O') 0x00 = 'O'
        // 0x4F ('O') 0x01 = 'P' / 0x50
        // Thus, 0x4F should always be followed by a second term char
        // Thus, 0x50 should only ever appear as a term char 
        // Regardless of the LEN implementation(s)
        //
        // Further, the de-packing of stuffed/escaped chars happens in the 
        // packet parser, not the receiver here.
        //
        // Example behavior:
        // If overflow, send term char, P
        // 1st time with term char then gives invalid packet
        // 2nd time with term char gives completed packet (keeping just the amount within the allowed length)
        // this also then triggers clearing out the rxPacket after processing
        
        switch ( currState ) {

            case packetComplete:
                packetPositionIndex = 0;
            case receivingDataNotStarted:
                if ( c == START_OF_PACKET_MARKER ) {
                    rxPacket[packetPositionIndex++] = c;
                    nextState = receivingDataBuildingPacket;
                    // Eh? For below ... (?)
                    if(packetPositionIndex > sizeof(rxPacket)) {
                     nextState = packetOverflow;
                    }
                }
                break;
            case receivingDataBuildingPacket:
                if(c != END_OF_PACKET_MARKER) {
                    rxPacket[packetPositionIndex++] = c;
                    nextState = receivingDataBuildingPacket; 
                    if(packetPositionIndex > sizeof(rxPacket)) {
                     nextState = packetOverflow;
                    }
                } else {
                    // We are excluding the writing of the term char 
                    // into the rxPacket for now (?))
                    nextState = packetComplete;
                }
                break;
            case invalidPacket:
                if ( c == END_OF_PACKET_MARKER ) {
                    nextState = packetComplete;
                } else {
                    nextState = invalidPacket;
                }
                break;
            case packetOverflow:
                nextState = packetOverflow;
            default:
                if( c == END_OF_PACKET_MARKER ){
                    nextState = invalidPacket;
                }
                break;    
        } // end of state machine switch
        
        currState = nextState;

        retVal = (uint8_t) currState;
        
#ifdef SUPERDEBUG
        //printf("%s", "[PPI:");
        printf("[PacPosInd: %d]\r\n", packetPositionIndex);
        //putch('-');
        //printf("%s", "][RxCnt:");
        printf("[RxCnt: %d]\r\n", eusartRxCount);
        //putch(c);

        uint8_t i =0;
        uint8_t j;
        printf("%s", "[rxPacket: ");
        for ( i = 0; i < DEBUG_RX_PACKET_OUTPUT_SIZE; i++) { // PACKET_MAX_SIZE; i++ ) {
            j = rxPacket[i];
            //putch(j);
            printf("0x%02x ", j);
        }
        //printf("%s", "]\r\n[currState: ");
        printf("]\r\n[currState: %d = ", currState);
        printf("%s", debugRxStates[currState]);
        //putch(']');
        printf("%s", "]\r\n\r\n");
#endif 
        
    } // end of while there is Rx data to process/pull in

    return retVal;
    
} // end of rxDataHandler()



void processRxPacket() {
    
    // Actual packet contents processing
    // As returned from the rxDataHandler / rxDataParseAndAccum:
    // Will begin with start byte
    // Term char byte will not be included 
    
    // Currently, we are just implementing the LSBs of the command structure
    uint16_t cmd =  ((uint16_t)rxPacket[IDX_BCMD_CMD_MSB]    << 8) | (uint16_t)rxPacket[IDX_BCMD_CMD_LSB];
    uint16_t arg1 = ((uint16_t)rxPacket[IDX_BCMD_PARAM1_MSB] << 8) | (uint16_t)rxPacket[IDX_BCMD_PARAM1_LSB];
    uint16_t arg2 = ((uint16_t)rxPacket[IDX_BCMD_PARAM2_MSB] << 8) | (uint16_t)rxPacket[IDX_BCMD_PARAM2_LSB];
#ifdef SUPERDEBUG 
    printf("%s", "Debug: processRxPacket: cmd_fam_lsb, cmd, arg1, and arg2\r\n");
    printf("0x%02x\r\n", rxPacket[IDX_BCMD_CMD_FAM_LSB]);
    printf("0x%04x\r\n", cmd);
    printf("0x%04x\r\n", arg1);
    printf("0x%04x\r\n", arg2);
    printf("%s", "\r\n");
#endif 
    
    switch ( rxPacket[IDX_BCMD_CMD_FAM_LSB]) {
        case BCMD_CMD_FAM_LSB_TEST:
            printf("%s",c_tx_test_reply);
            break;
        case BCMD_CMD_FAM_TX:
            if ( processTxCmd(cmd, arg1, arg2) == OK ) {
                printf("[OK] %s", c_tx_cmd_fam_tx);
            }
            break;
        case BCMD_CMD_FAM_RX:
            if ( processRxCmd(cmd, arg1, arg2) == OK ) {
                printf("[OK] %s", c_tx_cmd_fam_rx);
            }
            break;
        case BCMD_CMD_FAM_PAQ:
            if ( processPaqCmd(cmd, arg1, arg2) == OK ) {
                printf("[OK] %s", c_tx_cmd_fam_paq);
            }
            break;
        case BCMD_CMD_FAM_ALT:
            if ( processAltCmd(cmd, arg1, arg2) == OK ) {
                printf("[OK] %s", c_tx_cmd_fam_alt);
            }
            break;
        default:
            printf("%s", c_tx_cmd_fam_err);
            //printRxPacket();
            break;
    }
    
}



int8_t processTxCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2) {
    
    switch(cmd) {
        case BCMD_CMD_LSB_SET_TX_CHAN:
            setTxChannel((uint8_t)arg1);
            break;
        case BCMD_CMD_LSB_SET_DAC_LEVEL:
            setDacOutput((uint8_t)arg1);
            break;
        default:
            printf("%s", c_tx_cmd_err);
            //printRxPacket();
            return ERR;
    }
    return OK;
}



int8_t processRxCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2) {
    
    switch (cmd) {
        case BCMD_CMD_LSB_SET_RX_CHAN:
            setRxChannel((uint8_t)arg1);
            break;
        case BCMD_CMD_LSB_SET_RX_GAIN:
            setRxGain((uint8_t)arg1);
            break;
        case BCMD_CMD_LSB_SET_RX_DELAY:
            setRxDelay((uint8_t)arg1);
            break;
        default:
            printf("%s", c_tx_cmd_err);
            //printRxPacket();
            return ERR;
            
    }
    
    return OK;
}



int8_t processPaqCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2) {
    
    switch (cmd) {
        
        case BCMD_CMD_LSB_PAQ_ONCE:
            g_modeIsContinuousPaq = false;
            g_modeIsChannelScan = false;
            // Just trigger this ISR once
            CONTINUOUS_PAQ_TMR_ISR();              
            break;
            
        case BCMD_CMD_LSB_PAQ_CONT_DEF:
            g_modeIsContinuousPaq = true;
            g_modeIsChannelScan = false;
            break;
            
        case BCMD_CMD_LSB_PAQ_CONT_SCAN:
            g_modeIsContinuousPaq = true;
            g_modeIsChannelScan = true;
            // base 1 from the control side
            g_currentRxChan = 1;    
            g_currentTxChan = 1;
            // Because the pre-PAQ ISR tries to preserve timing before pulse control
            // to achieve consistent waveform basis, we need to send the reset 
            // to the FPGA now to reset the waveform sequence numbering.  Then, 
            // following cycles will be reset just after the ISR body executes.
            resetWaveformSequenceCount();
            break;
        default:
            printf("%s", c_tx_cmd_err);
            //printRxPacket();
            return ERR;
    }
    
    return OK;
}



int8_t processAltCmd(uint16_t cmd, uint16_t arg1, uint16_t arg2) {
    
    adc_result_t r = 0x0000;
    
    switch (cmd) {
        case BCMD_CMD_LSB_GET_ADC_VAL:
            r = getAndSendAdcSample();
            //uint8_t lsb = r & 0x00ff; // or read the register directly?
            //uint8_t msb = ( r & 0xff00 ) >> 8; // same note as above?
            printf( "ADC:%u\r\n", (uint16_t)r );
            uint8_t msb = ADRESH;
            uint8_t lsb = ADRESL;
            printf("ADC: %d %d\r\n", msb, lsb);
            // Too much space:
            //float rf = (float)r * 3.3f / 0x3ff;
            //printf("ADC:%.3f\r\n", rf);
            break;
        default:
            printf("%s", c_tx_cmd_err);
            //printRxPacket();
            return ERR;
            
    }
    
    return OK;
}





/**
 End of File
*/