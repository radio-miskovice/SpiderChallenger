// The commands that can be sent from PC, either buffered or immediate:
#define CMD_FIRST 1
#define CMD_SET_PTT 1 // Toggle PTT
#define CMD_SET_KEY 2 // Toggle Key
#define CMD_SPEED_CHANGE 3
#define CMD_SET_LEAD_TIME 4
#define CMD_SET_TAIL_TIME 5
#define CMD_SET_HANG_TIME 6
#define CMD_SET_WEIGHTING 7
#define CMD_ENABLE_FEATURES 8 // Bitwise!
#define CMD_SET_PADDLES_TRIGGER_PTT 9
#define CMD_SET_SIDETONE_AUTOMATIC 10
#define CMD_SET_SIDETONE_MANUAL 11
#define CMD_SET_IAMBIC_MODE 12
// #define CMD_RESERVED 13
#define CMD_BREAK_IMMY 14
#define CMD_RESET 15
#define CMD_PING 16
#define CMD_GET_SIGNATURE 17
#define CMD_BEEP 18
#define CMD_SET_FEEDBACK 19
#define CMD_SET_LOW_LIMIT 20
#define CMD_SET_HIGH_LIMIT 21
#define CMD_SET_MANUAL_SENDING_LIMIT 22
#define CMD_SET_PADDLES_SWAPPED 23
#define CMD_SAVE_CONFIG 24
#define CMD_STORE_MSG 25
#define CMD_LAST 25

// Every immediate command must preceeded with this char:
#define ESCAPE_CHAR 27