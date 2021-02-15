#define EEPROM_PUT_NEXT(x,ptr) EEPROM.put(ptr,x); ptr += sizeof(x) ;
#define EEPROM_GET_NEXT(x, ptr) EEPROM.get(ptr, x);  ptr += sizeof(x);
