/**** Challenger keyer EEPROM configuration functions ****/

#define EEPROM_PUT_NEXT(x,ptr) EEPROM.put(ptr,x); ptr += sizeof(x) ;
#define EEPROM_GET_NEXT(x, ptr) EEPROM.get(ptr, x);  ptr += sizeof(x);

void save_config(bool erase);
void load_config();