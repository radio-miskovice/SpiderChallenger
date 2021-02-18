/*  Spider Keyer (version 1.xx) by Petr OK1FIG, Dec 2018, May 2019, Dec 2020
    This code was developed for Arduino Nano. Other Arduinos were not tested.
    The goal is to work seamlessly with HamRacer contesting software:
    http://ok1fig.nagano.cz/HamRacer/HamRacer.htm
    For more details on the Spider Keyer go here:
    http://ok1fig.nagano.cz/SpiderKeyer/SpiderKeyer.htm
*/

#include <EEPROM.h>
#include "features.h"
#include "pins.h"
#include "ptt.h"
#include "core.h"
#include "morse.h"
#include "command.h"       // command definitions
#include "eeprom_config.h" // configuration load and save
#include "paddles.h"       // paddle interrupt functions and variables
#include "speed_control.h" // integrates potentiometer or rotary encoder to control speed

// TODO: place header file here
unsigned int checkAnalogButton();
extern bool buttonEvent;

// internally used constants

#define KEY_UP 0
#define KEY_DOWN 1

// VARIABLE DECLARATIONS

/* SPEED */
unsigned int wpm = 25;    // current speed in WPM
byte wpm_limit_low = 12;  // lowest possible speed in WPM
byte wpm_limit_high = 40; // highest possible speed in WPM
// unsigned int wpm_manual = 25; // current paddle-keying speed in WPM
unsigned int wpm_auto = 25;      // current auto-send speed in WPM
unsigned int wpm_limit_man = 0;  // maximum speed for paddle keying in WPM. 0: switched off
unsigned int wpm_dif = 0;        // Difference introduced by a buffered speed change command DEPRECATE
unsigned int wpm_dif_manual = 0; // Difference to get slower manual sending DEPRECATE
bool iambic_flag = false;        // true if paddles squeezed
bool wpm_set_manual_flag = true; // true if manual speed control is enabled (rotary or potentiometer)

/* KEYER MODE AND KEYING PARAMETERS */
byte keyer_mode = IAMBIC_B;              // IAMBIC A, B
unsigned int dah_to_dit_ratio_pct = 300; // dah length vs dit in per cent; default is 300
byte wordspace_length_units = 6;
byte letterspace_length_units = 3;
byte weighting_pct = 50; // weighting in per cent 50 = 50 pct
bool paddles_swapped = false;

/* SENDING MODE */
volatile byte sending_mode = SEND_MODE_PADDLE; // SEND_MODE_MAN, SEND_MODE_AUTO
byte sending_mode_last = SEND_MODE_PADDLE;
bool paddles_trigger_ptt = false; // Paddles are allowed to trigger PTT, or not
bool commandMode = false ;

/* KEYING STATE */
byte dit_buffer = 0;
byte dah_buffer = 0;
byte being_sent = SENDING_NOTHING; // SENDING_NOTHING, SENDING_DIT, SENDING_DAH
byte key_state = KEY_UP;           // 0 = key up, 1 = key down
byte key_forced_down = KEY_UP;     // Low-level key down (via immy command)  0: key up; 1: key down; 2 : key+ptt down
bool letterspace_pending = false;
bool wordspace_pending = false;

/* SIDETONE SETTINGS */
unsigned int sidetone_hz_auto = 700; // In Hz
unsigned int sidetone_hz_man = 700;  // In Hz

/* PTT timing and state */
unsigned int ptt_tail_time = 30;
unsigned int ptt_lead_time = 5;
unsigned int ptt_hang_time_wsu_pct;
byte ptt_state; // 0: RX, 1: TX
unsigned long ptt_time;
bool ptt_forced_on; // Low-level PTT down (via immy command)

/* SERIAL */
byte serial_byte_rcvd;
byte serial_esc_char_rcvd_command;
byte serial_send_buffer[256]; // Standard buffer for online transmitting. Must be exatly 256 as indexes are overflowing bytes
byte serial_buffer_head;      // Points to buffer where to store next char
byte serial_buffer_tail;      // Poinst to buffer from where to get the next char
byte serial_cmd_buffer[2];    // Immediate command buffer
bool send_feedback = false ;
byte enabled_features = 7 ; // bits: 0 (val 1): ptt, 1 (val 2): key, 2 (val 4): speed PIN_POTENTIOMETER, ..., ... 7: ...
unsigned long last_event;
long btn_toggle; // Debounce the message sending button
unsigned long last_powerbank;

/**
 * Initial setup 
 */
void setup()
{
  pinMode(PIN_MODE_LED, OUTPUT);
  digitalWrite(PIN_MODE_LED, HIGH);
  pinMode(PIN_PADDLE_LEFT, INPUT_PULLUP);
  pinMode(PIN_PADDLE_RIGHT, INPUT_PULLUP);
  pinMode(PIN_KEY_OUT, OUTPUT);
  digitalWrite(PIN_KEY_OUT, LOW);

  // pinMode (dummy_1, OUTPUT);
  // pinMode (dummy_2, OUTPUT);
  // digitalWrite (dummy_1, LOW);
  // digitalWrite (dummy_2, LOW);

  // pinMode (button, INPUT_PULLUP);

  ptt_init();
  pinMode(PIN_SIDETONE, OUTPUT);
  digitalWrite(PIN_SIDETONE, LOW);
  setup_speed_control();
  switch_mode_paddles();
  wpm = get_wpm_from_controls(); // in case of potentiometer, read potentiometer, otherwise use initial values
  load_config();
  reset_com();

  Serial.begin(57600); // primary_serial_port_baud_rate

  // attachInterrupt(digitalPinToInterrupt(PIN_PADDLE_LEFT), left_paddle_change, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(PIN_PADDLE_RIGHT), right_paddle_change, CHANGE);

  Serial.println("spider>");
  // Short low-pitch beep to indicate Arduino Nano was started/restarted:
  tone(PIN_SIDETONE, 512);
  delay(46);
  tone(PIN_SIDETONE, 1024);
  delay(230);
  noTone(PIN_SIDETONE);
  digitalWrite(PIN_MODE_LED, LOW);
}

/* Main loop */
void loop()
{
  checkAnalogButton();
  if (buttonEvent)
  {
    byte x = 1 - digitalRead(PIN_MODE_LED);
    digitalWrite(PIN_MODE_LED, x);
    if (!x)
    {
      tone(PIN_SIDETONE, 2048);
      delay(125);
      tone(PIN_SIDETONE, 256);
      delay(125);
      noTone(PIN_SIDETONE);
      commandMode = false ;
    }
    else
    {
      tone(PIN_SIDETONE, 256);
      delay(125);
      tone(PIN_SIDETONE, 2048);
      delay(125);
      noTone(PIN_SIDETONE);
      commandMode = true ;
    }
    buttonEvent = false;
  }
  check_paddle(DIT);
  check_paddle(DAH);
  service_paddle_buffers();
  check_serial();
  service_paddle_buffers();
  service_send_buffer();
  check_ptt_tail();
  check_speed_control();
  check_delayed_send_response();
  check_button();
  check_keyed_char();
}

void echo_char( char x ) {
  // if( sending_mode == SEND_MODE_PADDLE && x != 0 ) {
  //   Serial.write(x) ;
  // }
}

void check_keyed_char()
{
  if (sending_mode != SEND_MODE_PADDLE)
    return;
  unsigned long current = (millis() - last_element_ms) * 100;
  unsigned int unit = 1200 / wpm;
  if (letterspace_pending && (current / unit) > 200)
  {
    char x = get_last_paddle_morse();
    if (x > 0)
    {
      x = morse2ascii( x );
      echo_char( x );
      if( commandMode ) {
        if( x == 'N' ) {
          paddles_swapped = !paddles_swapped ;
        }
      }
      letterspace_pending = false;
      wordspace_pending = true;
    }
    return;
  }
  if (wordspace_pending && (current / unit) > 500)
  {
    wordspace_pending = false;
    echo_char(' ');
  }
}

/* RESET ALL CONTROLS AND OUTPUTS */
void reset_com()
{
  /* Makes a total reset, puts all to inactive and resets the status flags. Should be called
    in need to fetch the device to a "defined state". */
  digitalWrite(PIN_KEY_OUT, LOW);
  ptt_off();
  noTone(PIN_SIDETONE);
  serial_esc_char_rcvd_command = 0;
  ptt_forced_on = false;
  key_forced_down = 0;
  serial_buffer_head = 0;
  serial_buffer_tail = 0;
  dit_buffer = 0; // Used for buffering paddle hits in iambic operation
  dah_buffer = 0; // Used for buffering paddle hits in iambic operation
  being_sent = 0; // One of: SENDING_NOTHING, SENDING_DIT, SENDING_DAH
  key_state = 0;  // 0 = key up, 1 = key down
  ptt_time = 0;   // Time when PTT was set on
  ptt_state = 0;
  iambic_flag = false;
  wpm_dif = 0;
  wpm_dif_manual = 0;
  paddles_touched = false;
  sending_mode_last = SEND_MODE_PADDLE;
  sending_mode = SEND_MODE_PADDLE;
  last_event = 0;
}

/**
 * Check serial port 
 * **/
void check_serial()
{
  int bytes_rcvd = 0;
  while ((Serial.available() > 0))
  {
    serial_byte_rcvd = Serial.read();
    Serial.write(serial_byte_rcvd);
    check_com();
    if (bytes_rcvd == 0)
    {
      delay(10);       // Enable the following chars to be received in one row
      send_response(); // When HR sends a string, it awaits a confirmation within some 250 mSec, or so
    }                  // In order the lead time doesn't occur between first and second char
    bytes_rcvd++;
  }
}

/** 
 * 
 * */
void service_send_buffer()
{
  if (send_buffer_bytes() > 0)
  {
    if (serial_send_buffer[serial_buffer_tail] >= 32)
    { // Printable character, send it
      send_ascii_char(serial_send_buffer[serial_buffer_tail]);
      remove_from_send_buffer();
    }
    else if ((serial_send_buffer[serial_buffer_tail] >= CMD_FIRST) && (serial_send_buffer[serial_buffer_tail] <= CMD_LAST))
    {
      if (send_buffer_bytes() > 1)
      {
        process_command(serial_send_buffer[serial_buffer_tail], serial_send_buffer[(serial_buffer_tail + 1) % 256], true); // todo:
        remove_from_send_buffer();
        remove_from_send_buffer();
      }
    }
  }
}

/**
 * 
 **/
void speed_set(int wpm_set)
{
  if (wpm_set < wpm_limit_low)
    wpm_set = wpm_limit_low;
  if (wpm_set > wpm_limit_high)
    wpm_set = wpm_limit_high;
  wpm = wpm_set;
  send_response();
}

/**
 * KEY UP OR DOWN
 * Acts on PIN_KEY_OUT
 * @param state 0 for key up, 1 for key down
 * */
void key(int state)
{
  if ((state) && (key_state == 0))
  {
    byte ptt_state_previous = ptt_state;

    if (key_forced_down != 1)
      if (paddles_trigger_ptt || sending_mode == SEND_MODE_AUTO)
      {
        ptt(true);
      }

    if (FEATURE_ENABLED(enabled_features, FLAG_ENABLE_KEY) && !commandMode ) 
      digitalWrite(PIN_KEY_OUT, HIGH);

    if (sending_mode == SEND_MODE_PADDLE)
    {
      if (sidetone_hz_man > 0)
        tone(PIN_SIDETONE, sidetone_hz_man);
    }
    else
    {
      if (sidetone_hz_auto > 0)
        tone(PIN_SIDETONE, sidetone_hz_auto);
    }
    key_state = 1;
  }
  else
  {
    if ((state == 0) && (key_state))
    {
      digitalWrite(PIN_KEY_OUT, LOW);
      if (key_forced_down != 1)
        if (paddles_trigger_ptt || sending_mode == SEND_MODE_AUTO)
          ptt(true);
    }
    if (sidetone_hz_auto > 0 || sidetone_hz_man > 0)
      noTone(PIN_SIDETONE);
    key_state = 0;
  }
  check_ptt_tail();
}
// --------------------------------------------------------------------------------------------

/**
 * Timing loop for one morse element (DIT, DAH or element pause).
 * @param time loop length in percent of one element unit (default DIT = 100, default DAH = 300)
 * @param speed_wpm speed in wpm
 * 
 * Unit duration in milliseconds is calculated as 1200 / WPM.
 **/
void control_element_duration(unsigned int length, int speed_wpm)
{
  unsigned long ticks = 12 * length / speed_wpm;
  unsigned long start = millis();
  while (((millis() - start) < ticks))
  {
    check_ptt_tail();
    iambic_flag = (keyer_mode == IAMBIC_A) && (digitalRead(PIN_PADDLE_LEFT) == LOW) && (digitalRead(PIN_PADDLE_RIGHT) == LOW);

    if (being_sent == SENDING_DIT)
    {
      check_paddle(DAH);
    }
    else
    {
      if (being_sent == SENDING_DAH)
      {
        check_paddle(DIT);
      }
      else
      {
        check_paddle(DAH);
        check_paddle(DIT);
      }
    }

    if (sending_mode == SEND_MODE_AUTO && (digitalRead(PIN_PADDLE_LEFT) == LOW || digitalRead(PIN_PADDLE_RIGHT) == LOW || dit_buffer || dah_buffer))
    {
      switch_mode_paddles();
      sending_mode_last = SEND_MODE_PADDLE;
      return;
    }
  }

  if ((keyer_mode == IAMBIC_A) && (iambic_flag) && (digitalRead(PIN_PADDLE_LEFT) == HIGH) && (digitalRead(PIN_PADDLE_RIGHT) == HIGH))
  {
    iambic_flag = 0;
    dit_buffer = 0;
    dah_buffer = 0;
  }
} // loop_element_length_integer

// --------------------------------------------------------------------------------------------
void service_paddle_buffers()
{
  if ((keyer_mode == IAMBIC_A) && (iambic_flag) && (digitalRead(PIN_PADDLE_LEFT)) && (digitalRead(PIN_PADDLE_RIGHT)))
  {
    iambic_flag = 0;
    dit_buffer = 0;
    dah_buffer = 0;
  }
  else
  {
    if (dit_buffer)
    {
      dit_buffer = 0;
      switch_mode_paddles();
      send_morse_element(DIT);
    }
    if (dah_buffer)
    {
      dah_buffer = 0;
      switch_mode_paddles();
      send_morse_element(DAH);
    }
  }
}

void ptt(bool state)
{
  if (state != ptt_state)
  {
    ptt_state = state;
    if (state)
    {
      if (FEATURE_ENABLED(enabled_features, FLAG_ENABLE_PTT))
        ptt_on();
      last_event = millis(); // Use a delayed reporting instead
      delay(ptt_lead_time);
    }
    else
    {
      ptt_off();
      wpm_dif = 0; // Relasing PTT will cancel buffered speed change
      last_event = millis();
    }
  }
  if (state)
    ptt_time = millis();
}

void check_ptt_tail()
{
  if (key_state)
  {
    ptt_time = millis();
  }
  else
  {
    if ((ptt_state) && (!ptt_forced_on))
    {
      if (sending_mode_last == SEND_MODE_PADDLE)
      {
        if ((millis() - ptt_time) >= (((12 * wordspace_length_units * ptt_hang_time_wsu_pct / wpm)) + ptt_tail_time))
        {
          ptt(false);
        }
      }
      else
      {
        if ((millis() - ptt_time) > ptt_tail_time)
        {
          if (send_buffer_bytes() == 0)
            ptt(false);
        }
      }
    }
  }
}

// --------------------------------------------------------------------------------------------

/**
 * @param paddle ==0: check dit, !=0 : check dah
 * */
void check_paddle(byte paddle)
{
  byte pin = (paddle == DIT)
                 ? (paddles_swapped ? PIN_PADDLE_LEFT : PIN_PADDLE_RIGHT)
                 : (paddles_swapped ? PIN_PADDLE_RIGHT : PIN_PADDLE_LEFT);
  byte paddle_state = digitalRead(pin);
  if (paddle_state == 0)
  { // paddle squeezed, do something
    if (paddle == DIT)
      dit_buffer = 1;
    else
      dah_buffer = 1;
    paddles_touched = true;
    key_forced_down = 0;
    // Update last_event only if it was set by ptt():
    if (last_event > 0)
      last_event = millis();
    if (wpm > wpm_limit_man && wpm_limit_man > 0)
      wpm_dif_manual = wpm_limit_man - wpm;
  }
}

// --------------------------------------------------------------------------------------------
void clear_send_buffer()
{
  serial_buffer_head = 0;
  serial_buffer_tail = 0;
  send_response();
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void remove_from_send_buffer()
{
  if (serial_buffer_tail != serial_buffer_head)
  {
    serial_buffer_tail++;
    if (serial_buffer_tail == serial_buffer_head) // Last char was just removed, send response
      send_response();
  }
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void add_to_send_buffer(byte serial_byte_rcvd)
{
  paddles_touched = false;
  serial_send_buffer[serial_buffer_head] = serial_byte_rcvd;
  serial_buffer_head++;
}

// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void process_command(byte command, byte data, bool buffered_command)
{
  byte i, j;
  switch (command)
  {
  case CMD_SET_PTT: // PTT
    clear_send_buffer();
    if (data)
    {
      ptt_forced_on = true;
      ptt(true);
    }
    else
    {
      ptt(false);
      ptt_forced_on = false;
    }
    break;
  case CMD_SET_KEY: // Key
    clear_send_buffer();
    key_forced_down = data; // 0, 1, or 2
    if (data > 0)
    {
      key(true);
      paddles_touched = false;
    }
    else
    {
      key(false);
    }
    send_response(); // Response is sent only when KEY is toggled manually
    break;
  case CMD_SPEED_CHANGE: // Speed
    if (data == 255)
    { // Value of 255 returns to the value of PIN_POTENTIOMETER
      wpm_set_manual_flag = true;
      speed_set(get_wpm_from_controls());
      // speed_set(pot_value_wpm());
    }
    else
    {
      if (data == 0) // Value 0 clears the buffered speed change
        wpm_dif = 0;
      else if (buffered_command) // Other value, if buffered, changes speed temporarily
        wpm_dif = data - wpm;
      else
      {
        speed_set(data); // First, report back the changed speed
        wpm_set_manual_flag = false;
        speed_set(data); // Second, report that it was switched to the PC-controlled way
      }
    }
    break;
  case CMD_BREAK_IMMY: // Speed
    reset_com();
    break;
  case CMD_SET_LEAD_TIME:
    ptt_lead_time = data * 5;
    break;
  case CMD_SET_TAIL_TIME:
    ptt_tail_time = data * 5;
    break;
  case CMD_SET_HANG_TIME:
    ptt_hang_time_wsu_pct = data;
    break;
  case CMD_SET_WEIGHTING:
    weighting_pct = data;
    break;
  case CMD_ENABLE_FEATURES:
    enabled_features = data; // This is bitwise!
    break;
  case CMD_SET_PADDLES_TRIGGER_PTT:
    reset_com();
    paddles_trigger_ptt = data;
    break;
  case CMD_SET_SIDETONE_AUTOMATIC:
    sidetone_hz_auto = data * 10;
    break;
  case CMD_SET_SIDETONE_MANUAL:
    sidetone_hz_man = data * 10;
    break;
  case CMD_SET_IAMBIC_MODE:
    if (data == 0)
    {
      keyer_mode = IAMBIC_A;
    }
    else if (data == 1)
    {
      keyer_mode = IAMBIC_B;
    }
    break;
  case CMD_RESET:
    setup();
    break;
  case CMD_PING:
    send_response_bytes();
    break;
  case CMD_BEEP:
    tone(PIN_SIDETONE, 750);
    delay(10);
    noTone(PIN_SIDETONE);
    break;
  case CMD_GET_SIGNATURE:
    Serial.print("Spider Keyer (Arduino Nano) by OK1FIG [");
    Serial.print(VERSION);
    Serial.print("]"); // Don't change this to work seamlessly with HamRacer
    break;
  case CMD_SET_FEEDBACK:
    send_feedback = data > 0;
    send_response();
    break;
  case CMD_SET_LOW_LIMIT:
    if (data == 0)
    { // Value of 0 returns to the default value
      wpm_limit_low = 15;
    }
    else
    {
      wpm_limit_low = data;
    }
    break;
  case CMD_SET_HIGH_LIMIT:
    if (data == 0)
    { // Value of 0 returns to the default value
      wpm_limit_high = 40;
    }
    else
    {
      wpm_limit_high = data;
    }
    break;

  case CMD_SET_MANUAL_SENDING_LIMIT:
    wpm_limit_man = data;
    break;

  case CMD_SET_PADDLES_SWAPPED:
    paddles_swapped = data != 0;
    break;

  case CMD_SAVE_CONFIG:
    save_config(!data); // data == 0 will erase old configuration
    break;

  case CMD_STORE_MSG:
    store_message(data);
    break;
  }
}

// --------------------------------------------------------------------------------------------
void check_com()
{
  if (serial_esc_char_rcvd_command == 0)
  {
    if (serial_byte_rcvd != ESCAPE_CHAR)
    {
      add_to_send_buffer(serial_byte_rcvd); // Add even CR, it serves as a flag in the buffer
    }
    else
    { // ESC arrived
      serial_esc_char_rcvd_command = 1;
    }
  }
  else if (serial_esc_char_rcvd_command == 1)
  { // Esc char passed, now it is the command byte
    serial_cmd_buffer[0] = serial_byte_rcvd;
    serial_esc_char_rcvd_command++;
  }
  else
  {
    serial_cmd_buffer[1] = serial_byte_rcvd;
    process_command(serial_cmd_buffer[0], serial_cmd_buffer[1], false);
    serial_esc_char_rcvd_command = 0;
  }
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void send_response()
{
  if (send_feedback)
  {
    send_response_bytes();
  }
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void send_response_bytes()
{
  byte byte1 = 0;
  byte byte2 = 0;
  bitSet(byte1, 7); // The highest bit is always 1

  if (false)          //RESERVED
    bitSet(byte1, 6); //

  if ((send_buffer_bytes() > 0) || (serial_esc_char_rcvd_command > 0))
    bitSet(byte1, 5); // any chars buffered?

  if (ptt_state)
    bitSet(byte1, 4); // PTT currently down

  if (key_state)
    bitSet(byte1, 3); // Key currently down

  if (paddles_touched)
    bitSet(byte1, 2); // Sending was interrupted by touching the paddles

  // For the speed report, use only lower 6 bits
  if (wpm_set_manual_flag)
    if (wpm <= 64) // MSB bit must remain 0
      byte2 = wpm;

  Serial.write(byte1);
  Serial.write(byte2);
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
byte send_buffer_bytes()
{ // Number of chars in the buffer to send
  if (serial_buffer_head >= serial_buffer_tail)
    return serial_buffer_head - serial_buffer_tail;
  else
    return serial_buffer_head - serial_buffer_tail % 256;
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void check_delayed_send_response()
{
  /* Probably due to interrupts handling it is not desirable to call Serial.write() while the hand keying is in progress. 
   It causes time irregularities. */
  /* This sends response bytes after one wordspace of inactivity. During this time:
 - paddles were not touched
 - PTT has not been toggled  
 */
  if ((last_event != 0) && (millis() > last_event + ((12 * wordspace_length_units * ptt_hang_time_wsu_pct / wpm) + ptt_tail_time)))
  {
    last_event = 0;
    send_response();
  }
}

void store_message(byte data)
{
  // Stores button-message to EEPROM. Use half of the EEPROM size, start at the middle, at 256:
  int i;
  if (data == 0)
  { // Mark the whole as unused
    for (i = 256; i < 512; i++)
    {
      EEPROM.write(i, 0xFF);
    }
  }
  else
  {
    i = 256;
    while (EEPROM.read(i) != 0xFF && i < 511)
      i++; // Find first unoccupied position
    if (i < 512)
      EEPROM.write(i, data);
  }
}
// --------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------
void send_message()
{
  // Copies the button-message from EEPROM to the standard to-be-sent buffer:
  int i = 256;
  while (EEPROM.read(i) != 0xFF && i < 512)
  {
    add_to_send_buffer(EEPROM.read(i));
    i++;
  }
}
// --------------------------------------------------------------------------------------------

// Inline utility functions

inline void setup_speed_control()
{
#ifdef USE_POTENTIOMETER
  pinMode(PIN_POTENTIOMETER, INPUT);
#elif defined(USE_ROTARY_ENCODER)
  pinMode(PIN_ROTARY_CLOCK, INPUT_PULLUP);
  pinMode(PIN_ROTARY_VALUE, INPUT_PULLUP);
  rotary_interrupt_enable();
#endif
}

inline void check_button() {}

inline void switch_mode_paddles()
{
  sending_mode = SEND_MODE_PADDLE;
  disable_paddle_interrupt();
}
