/**
 * Message buffer class and instance
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/

#define BUFFER_LENGTH 256

class MessageBuffer {
  public:
    static const unsigned char maxlength = BUFFER_LENGTH;

  private:
    char buffer[BUFFER_LENGTH];
    unsigned char head ;
    unsigned char tail ;
  public:
    MessageBuffer() {
      head = 0;
      tail = 0;
    }
    unsigned char free() {
      return (tail - head) ;
    }
    unsigned char length() {
      return (head - tail) ;
    }
    void put( char x ) {
      if( free() > 1 ) {
        buffer[ head++ ] ;
      }
    }
    char get() {
      if( length() > 0 ) return buffer[tail++] ;
      else return 0 ;
    }
};

MessageBuffer message() ;
