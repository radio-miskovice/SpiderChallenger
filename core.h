
/** SPIDER KEYER VARAIBLES **/
#include <stdint.h>

#define VERSION "1.31" // Dec 2020
#define MY_MAGIC_HEADER 0xCCCC // Binary 1010 1010 1010 1010

// Values used internally:
#define IAMBIC_A 1
#define IAMBIC_B 2
#define SENDING_NOTHING 0
#define SENDING_DIT 1
#define SENDING_DAH 2
#define SEND_MODE_UNDEF   0 // mode unknown
#define SEND_MODE_AUTO    1 // automatic mode 
#define SEND_MODE_PADDLE  2 // manual mode (nothing in buffer)

#define FLAG_ENABLE_PTT 0  // ptt control enabled
#define FLAG_ENABLE_KEY 1  // keying enabled
#define FLAG_ENABLE_SPEED_CTL 2  // set speed by manual control enabled
#define FEATURE_ENABLED( features, bit ) \
   ((features) & (1 << (bit)) >> (bit))

void setup();
void loop();
void reset_com();
void set_defaults();
void check_potentiometer();
void check_serial();
void service_send_buffer();
void speed_set(int wpm_set);
void send_dit();
void send_dah();
void key(int state);
void loop_element_lengths(float lengths, int speed_wpm_in);
void service_dit_dah_buffers();
byte pot_value_wpm();
void ptt(bool state);
void check_ptt_tail();
void check_dit_paddle();
void check_dah_paddle();
void send_the_dits_and_dahs(String const  char_to_send);
void send_char(byte cw_char);
void clear_send_buffer();
void remove_from_send_buffer();
void add_to_send_buffer(byte serial_byte_rcvd);
void process_command(byte command, byte data, bool buffered_command );
void check_com();
void send_response();
void send_response_bytes(); 
byte send_buffer_bytes();
void check_delayed_send_response();
void left_paddle_change();
void right_paddle_change();
void save_config(byte data);
void load_config();
void check_button();
void send_message();
