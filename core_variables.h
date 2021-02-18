#include <Arduino.h>

// Core script variables for reference in other source files

/* SPEED */
extern unsigned int wpm; // CW speed
extern byte wpm_limit_low;
extern byte wpm_limit_high;
extern unsigned int wpm_limit_man;  // Speed limit for manual sending, in wpm. 0: switched off
extern unsigned int wpm_dif;        // Difference introduced by a buffered speed change command
extern unsigned int wpm_dif_manual; // Difference to get slower manual sending
extern unsigned int wpm_manual ;    // current paddle-keying speed in WPM
extern unsigned int wpm_auto;       // current auto-send speed in WPM
extern bool iambic_flag;            // true if paddles squeezed
extern bool wpm_set_manual_flag;    // true if manual speed control is enabled (rotary or potentiometer)

/* KEYER MODE AND KEYING PARAMETERS */
extern byte keyer_mode; // IAMBIC A, B
extern unsigned int dah_to_dit_ratio_pct;
extern byte wordspace_length_units;
extern byte letterspace_length_units;
extern byte weighting_pct;
extern bool paddles_swapped;
extern bool commandMode ;

    /* SENDING MODE */
    extern volatile byte sending_mode; // MANUAL_SENDING, AUTOMATIC_SENDING
extern byte sending_mode_last;
extern bool paddles_trigger_ptt; // Paddles are allowed to trigger PTT, or not

/* KEYING STATE */
extern byte dit_buffer;
extern byte dah_buffer;
extern byte being_sent; // SENDING_NOTHING, SENDING_DIT, SENDING_DAH
extern byte key_state;  // 0 = key up, 1 = key down
extern byte key_forced_down; // Low-level key down (via immy command)  0: key up; 1: key down; 2 : key+ptt down
extern volatile bool paddles_touched;
extern bool letterspace_pending ;
extern bool wordspace_pending ;

/* POTENTIOMETER */
extern byte pot_wpm_read_last;
extern unsigned long last_pot_check_time;
extern int pot_full_scale_reading;
extern bool speed_set_by_pot;

/* SIDETONE SETTINGS */
extern unsigned int sidetone_hz_auto; // In Hz
extern unsigned int sidetone_hz_man;  // In Hz

/* PTT timing and state */
extern unsigned int ptt_tail_time;
extern unsigned int ptt_lead_time;
extern unsigned int ptt_hang_time_wsu_pct;
extern byte ptt_state; // 0: RX, 1: TX
extern unsigned long ptt_time;
extern bool ptt_forced_on; // Low-level PTT down (via immy command)

/* SERIAL */
extern byte serial_byte_rcvd;
extern byte serial_esc_char_rcvd_command;
extern byte serial_send_buffer[256]; // Standard buffer for online transmitting. Must be exatly 256 as indexes are overflowing bytes
extern byte serial_buffer_head;      // Points to buffer where to store next char
extern byte serial_buffer_tail;      // Poinst to buffer from where to get the next char
extern byte serial_cmd_buffer[2];    // Immediate command buffer
extern bool send_feedback;
extern byte enabled_features; // bits: 0 (val 1): ptt, 1 (val 2): key, 2 (val 4): speed PIN_POTENTIOMETER, ..., ... 7: ...
extern unsigned long last_event;
extern long btn_toggle; // Debounce the message sending button
extern unsigned long last_powerbank;
