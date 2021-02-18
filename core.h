/**** Challenger keyer core constants and function declarations ****/
#include <stdint.h>

#define VERSION "1.31" // Dec 2020
#define MY_MAGIC_HEADER 0xCCCC // Binary 1010 1010 1010 1010


// morse element constant
#define DIT 0
#define DAH 1

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

// Main function 
void setup();
void loop();

// Paddles and keying
void check_paddle(byte paddle);
void control_element_duration(unsigned int length, int speed_wpm);
void key(int state);
void service_paddle_buffers();

// PTT handling 
void ptt(bool state);
void check_ptt_tail();

// Buffer and memory handling
void service_send_buffer();
void clear_send_buffer();
void remove_from_send_buffer();
void add_to_send_buffer(byte serial_byte_rcvd);
byte send_buffer_bytes();
void send_message();

// Speed control
void speed_set(int wpm_set);

// Serial communication
void check_serial();
void send_response();
void send_response_bytes(); 
void check_delayed_send_response();

// Command processor
void process_command(byte command, byte data, bool buffered_command );
void check_com();
void check_button();

void set_defaults();
void reset_com(); 
