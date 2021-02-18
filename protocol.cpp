/**
 * Abstract protocol class for incoming stream handling
 * Spider Keyer protocol handler class
 * Spider Keyer protocol designed by Petr, OK1FIG
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
class Protocol
{
  public:
    static const unsigned char maxLength = 8 ;
  private: 
    char buffer[maxLength];
    unsigned char tail;

public:
  Protocol()
  {
    tail = 0;
  }

  unsigned char length()
  {
    return tail;
  }

  void reset()
  {
    tail = 0;
  }

  unsigned char put(char x)
  {
    buffer[tail++] = x;
  }

  char get(unsigned char i)
  {
    if (i < tail)
    {
      return buffer[i];
    }
    else
      return 0;
  }
};

class SpiderProtocol : Protocol
{
};
