#ifndef COMMAND_H
#define COMMAND_H

// The commands that can be sent from PC, either buffered or immediate:
#define CMD_FIRST 1
#define CMD_SET_PTT 1 // Toggle PTT Ctrl-A
#define CMD_SET_KEY 2 // Toggle Key Ctrl-B
#define CMD_SPEED_CHANGE 3   // Ctrl-C
#define CMD_SET_LEAD_TIME 4  // Ctrl-D
#define CMD_SET_TAIL_TIME 5  // Ctrl-E
#define CMD_SET_HANG_TIME 6  // Ctrl-F
#define CMD_SET_WEIGHTING 7  // Ctrl-G
#define CMD_ENABLE_FEATURES 8 // Ctrl-H Bitwise!
#define CMD_SET_PADDLES_TRIGGER_PTT 9 // Ctrl-I
#define CMD_SET_SIDETONE_AUTOMATIC 10 // Ctrl-J
#define CMD_SET_SIDETONE_MANUAL 11    // Ctrl-K
#define CMD_SET_IAMBIC_MODE 12        // Ctrl-L
// #define CMD_RESERVED 13  Ctrl-M, Enter
#define CMD_BREAK_IMMY 14    // Ctrl-N
#define CMD_RESET 15         // Ctrl-O
#define CMD_PING 16          // Ctrl-P
#define CMD_GET_SIGNATURE 17 // Ctrl-Q
#define CMD_BEEP 18          // Ctrl-R
#define CMD_SET_FEEDBACK 19  // Ctrl-S
#define CMD_SET_LOW_LIMIT 20    // Ctrl-T
#define CMD_SET_HIGH_LIMIT 21   // Ctrl-U
#define CMD_SET_MANUAL_SENDING_LIMIT 22 // Ctrl-V
#define CMD_SET_PADDLES_SWAPPED 23      // Ctrl-W
#define CMD_SAVE_CONFIG 24              // Ctrl-X
#define CMD_STORE_MSG 25                // Ctrl-Y
#define CMD_LAST 25

// Every immediate command must preceeded with this char:
#define ESCAPE_CHAR 27

#endif