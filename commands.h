/**
 * Please see README.md and LICENSE
 *
 * Copyright 2020 oshablue / nisch s. except where otherwise noted
 */

#ifndef COMMANDS_H
#define	COMMANDS_H

/*
 * [0] START
 * [1] CRC MSB (CRC OF LENGTH THRU LAST NON-STOP BYTE)
 * [2] CRC LSB
 * [3] LEN MSB (LENGTH AFTER LENGTH BYTES EXCL STOP BYTE)
 * [4] LEN LSB
 * [5] CMD FAMILY MSB
 * [6] CMD FAMILY LSB
 * [7] CMD MSB
 * [8] CMD LSB
 * [9] PARAM1 MSB
 * [10] PARAM1 LSB
 * [11] PARAM2 MSB
 * [12] PARAM2 LSB
 * [13 THROUGH 16-BIT MAX - 8]
 * [13 OR 13 + (LEN - 8)] STOP BYTE (MINIMUM LENGTH IS AT INDEX 13 FOR STOP BYTE)
 */

#define BCMD_START              0x53
#define BCMD_STOP               0x50
#define BCMD_ESCAPE             0x4F
#define BCMD_ESCAPE_O           0x00
#define BCMD_ESCAPE_P           0x01

#define CMD_BASE_LEN            14      // INCLUDING START AND STOP BYTES
#define CMD_CORE_LEN            8       // BYTES[5 - 12]

#define IDX_BCMD_CRC_MSB        1
#define IDX_BCMD_CRC_LSB        2
#define IDX_BCMD_LEN_MSB        3
#define IDX_BCMD_LEN_LSB        4

#define IDX_BCMD_CMD_FAM_MSB    5
#define IDX_BCMD_CMD_FAM_LSB    6
#define IDX_BCMD_CMD_MSB        7
#define IDX_BCMD_CMD_LSB        8
#define IDX_BCMD_PARAM1_MSB     9
#define IDX_BCMD_PARAM1_LSB     10
#define IDX_BCMD_PARAM2_MSB     11
#define IDX_BCMD_PARAM2_LSB     12


/*
 * Command Family: Test
 */
// Example: c_test_reply [CMD_BASE_LEN = 14] =
//                             CRC   CRC   LEN   LEN  CMDF  CMDF   CMD   CMD
//              { BCMD_START, 0x00, 0x00, 0x00, 0x08, 0x00, 0x01, 0x00, 0x01,
//
//                              P1    P1    P2    P2
//                            0x00, 0x00, 0x00, 0x00, BCMD_STOP }
// 0x53 0x00 0x00 0x00 0x08 0x00 0x01 0x00 0x01 0x00 0x00 0x00 0x00 0x50
// If testing with CoolTerm and its Send String Hex option, omit 0x:
// 53 00 00 00 08 00 01 00 01 00 00 00 00 50
#define BCMD_CMD_FAM_LSB_TEST       0x01
#define BCMD_CMD_LSB_TEST_REPLY     0x01
//extern const char c_tx_test_reply[];
extern const char * c_tx_test_reply;



/*
 * Command Family: TX
 */
//                            P1 P1 P2 P2
// 53 00 00 00 08 00 02 00 01 00 00 00 00 50
#define BCMD_CMD_FAM_TX             0x02
#define BCMD_CMD_LSB_SET_DAC_LEVEL  0x01
#define BCMD_CMD_LSB_SET_TX_CHAN    0x02
// TODO Set Tx Channel!!
extern const char * c_tx_cmd_fam_tx;


/*
 * Command Family: RX
 */
//                            P1 P1 P2 P2
// 53 00 00 00 08 00 03 00 01 00 00 00 00 50
// 53 00 00 00 08 00 03 00 02 00 00 00 00 50
#define BCMD_CMD_FAM_RX             0x03
#define BCMD_CMD_LSB_SET_RX_CHAN    0x01
#define BCMD_CMD_LSB_SET_RX_GAIN    0x02
#define BCMD_CMD_LSB_SET_RX_DELAY   0x03
extern const char * c_tx_cmd_fam_rx;

/*
 * Command Family: PAQ
 */
#define BCMD_CMD_FAM_PAQ            0x04
#define BCMD_CMD_LSB_PAQ_ONCE       0x01
#define BCMD_CMD_LSB_PAQ_CONT_DEF   0x02    // Default continuous PAQ
#define BCMD_CMD_LSB_PAQ_CONT_SCAN  0X03
//                            P1 P1 P2 P2
// 53 00 00 00 08 00 04 00 01 00 00 00 00 50
// 53 00 00 00 08 00 04 00 02 00 00 00 00 50
extern const char * c_tx_cmd_fam_paq;




/*
 * Command Family: ALT
 */
#define BCMD_CMD_FAM_ALT            0x05
#define BCMD_CMD_LSB_GET_ADC_VAL    0x01
//                            P1 P1 P2 P2
// 53 00 00 00 08 00 05 00 01 00 00 00 00 50
// 53 00 00 00 08 00 05 00 01 00 00 00 00 50
extern const char * c_tx_cmd_fam_alt;










// General replies
extern const char * c_tx_packet_overflow;
extern const char * c_tx_packet_invalid;
extern const char * c_tx_packet_receiving;
extern const char * c_tx_packet_received_ok;

// General Errors?
extern const char * c_tx_cmd_fam_err;
extern const char * c_tx_cmd_err;


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* COMMANDS_H */
