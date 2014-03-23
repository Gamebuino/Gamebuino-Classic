#include <EEPROM.h>

void setup()
{
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);

  EEPROM.write(0,'K');
  EEPROM.write(1,'i');
  EEPROM.write(2,'c');
  EEPROM.write(3,'k');
  EEPROM.write(4,' ');
  EEPROM.write(5,'a');
  EEPROM.write(6,'s');
  EEPROM.write(7,'s');
  EEPROM.write(10,121);

  EEPROM.write(12,5);
  EEPROM.write(13,' ');
  EEPROM.write(14,'r');
  EEPROM.write(15,'0');
  EEPROM.write(16,'d');
  EEPROM.write(17,'0');
  EEPROM.write(18,'t');
  EEPROM.write(19,' ');
  EEPROM.write(20,5);
  EEPROM.write(22,74);

  EEPROM.write(24,'I');
  EEPROM.write(25,' ');
  EEPROM.write(26,3);
  EEPROM.write(27,' ');
  EEPROM.write(28,'C');
  EEPROM.write(29,'A');
  EEPROM.write(30,'C');
  EEPROM.write(31,'A');
  EEPROM.write(34,26);

  EEPROM.write(36,'n');
  EEPROM.write(37,'0');
  EEPROM.write(38,'0');
  EEPROM.write(39,'b');
  EEPROM.write(46,9);

  /*EEPROM.write(48,'n');
   EEPROM.write(49,'0');
   EEPROM.write(50,'0');
   EEPROM.write(51,'b');
   EEPROM.write(58,9);*/

  // turn the LED on when we're done
  digitalWrite(13, HIGH);
}

void loop()
{
}

