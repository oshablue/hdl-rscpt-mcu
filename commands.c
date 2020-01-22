/**
 * Please see comments in main.c or in a readme.md if included or at oshablue.com
 */

#include "commands.h"

// Here we exclude the trailing \0 from length and definition
// and we allow the compiler to figure out the length
// For other legacy reasons, project previous was e.g.:
// const char c_tx_test_reply[16] = "TEST REPLY OK\r\n\0";
// We're 'normal' ish now.

// BIG OL' TODO:
// wrap and DRY - see SUPERDEBUG implementations etc.
// ALSO: Plenty of room for pairing back, customizing, etc.

// "CF" = Command Family -related
const char * c_tx_test_reply = "[OK]\r\n";
const char * c_tx_cmd_fam_tx = "CF:TX\r\n";
const char * c_tx_cmd_fam_rx = "CF:RX\r\n";
const char * c_tx_cmd_fam_paq = "CF:PAQ\r\n";
const char * c_tx_cmd_fam_alt = "CF:ALT\r\n";



// General replies
const char * c_tx_packet_overflow = "[ERR] RXPKT OFLW\r\n";
const char * c_tx_packet_invalid = "[ERR] RXPKT INV\r\n";
//const char * c_tx_packet_receiving = "[STAT] RX PKT RCVING...\r\n";
const char * c_tx_packet_received_ok = "[OK] RXPKT RCVD\r\n";


// General errors?
const char * c_tx_cmd_fam_err = "[ERR] CF\r\n";
const char * c_tx_cmd_err = "[ERR] C\r\n";