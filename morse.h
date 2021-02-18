// ASCII to Morse converter

extern unsigned long last_element_ms ;

void send_ascii_char(byte ascii);
// void send_morse_code(byte morse_code);
void send_morse_element(byte element);
// unsigned int ascii2code(char ascii);
unsigned char utf8_to_code( unsigned char prefix, unsigned char utf8char );
byte get_last_paddle_morse();
char morse2ascii(byte source);