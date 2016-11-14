#define write_flash_page (*((void(*)(const char * page, unsigned char * buffer))(0x7ffa/2)))
#define SETTINGS_PAGE ((const char *)(0x7000-128))

byte buffer[128];

void setup()
{
  Serial.begin(9600);
  Serial.print(F("Erasing settings..."));
  memset(buffer, 0, 128);
  write_flash_page (SETTINGS_PAGE, buffer);
  Serial.print(F(" done."));
  while(1);
}

void loop()
{
  
}
