#ifndef COMMAND_MODE_H
#define COMMAND_MODE_H

class CommandModeInterpreter {
  private:
    byte textLength = 0;
    char text[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    byte wpmSaved = 20 ;
  public:
    bool isActive = false ;
    void append( char c ) ;
    void service();
    void setup();
    void init();
    void exit();
};

extern CommandModeInterpreter commandMode ;

#endif