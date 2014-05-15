// Demo_readBinary (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the readBinary()-function.
//
// This demo will open a file named 'FILE.DAT' and
// display its contents in the Serial Monitor.
//
// SD card must be connected to the SPI port of your Arduino.
//
// Remember to select 115200 baud in the Serial Monitor.
//

#include <tinyFAT.h>
#include <avr/pgmspace.h>

byte res;
word result;
int i=0;

char *verboseError(byte err)
{
	switch (err)
	{
	case ERROR_MBR_READ_ERROR:
		return "Error reading MBR";
		break;
	case ERROR_MBR_SIGNATURE:
		return "MBR Signature error";
		break;
	case ERROR_MBR_INVALID_FS:
		return "Unsupported filesystem";
		break;
	case ERROR_BOOTSEC_READ_ERROR:
		return "Error reading Boot Sector";
		break;
	case ERROR_BOOTSEC_SIGNATURE:
		return "Boot Sector Signature error";
		break;
	default:
		return "Unknown error";
		break;
	}
}

void dumpBuffer(unsigned long start, int size)
{
  unsigned long val=0;
  char *str="xxxxxxxxxxxxxxxx";
  
  str[16]=0;
  for (int i=0; i<size; i++)
  {
    if ((i % 16)==0)
    {
      val=long(start+i);
      if (val < 0x10000000)
        Serial.print("0");
      if (val < 0x1000000)
        Serial.print("0");
      if (val < 0x100000)
        Serial.print("0");
      if (val < 0x10000)
        Serial.print("0");
      if (val < 0x1000)
        Serial.print("0");
      if (val < 0x100)
        Serial.print("0");
      if (val < 0x10)
        Serial.print("0");
      Serial.print(val, HEX);
      Serial.print(": ");
    }
    val=file.buffer[i];
    if (val < 0x10)
      Serial.print("0");
    Serial.print(val, HEX);
    Serial.print(" ");
    if ((val>31) and (val<128))
      str[i % 16]=val;
    else
      str[i % 16]=46;
    if ((i % 16)==15)
    {
      Serial.print("  ");
      Serial.println(str);
    }
  }
  if (size!=512)
  {
    for (int i=0; i<(16-(size % 16));i++)
    {
      Serial.print("   ");
    }
    Serial.print("  ");
    str[size % 16]=0;
    Serial.println(str);
  }
}


void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  Serial.println();
  // Initialize tinyFAT 
  // You might need to select a lower speed than the default SPISPEED_HIGH
  res=file.initFAT(); 
  if (res!=NO_ERROR)
  {
    Serial.print("***** ERROR: ");
    Serial.println(verboseError(res));
    while (true) {};
  }

  Serial.println("This demo will open a file named 'FILE.DAT' and display its contents in the Serial Monitor.");
  Serial.println();
  Serial.println("***** Send any character to start *****");
  while (!Serial.available()) {};
  Serial.flush();
  Serial.println();
  Serial.println();
  
  if (file.exists("FILE.DAT"))
  {  
    res=file.openFile("FILE.DAT", FILEMODE_BINARY);
    if (res==NO_ERROR)
    {
      result=512;
      while (result==512)
      {
        result=file.readBinary();
        switch(result)
        {
          case ERROR_WRONG_FILEMODE:
            Serial.println("** ERROR: Expected binary filemode...");
            break;
          case ERROR_NO_FILE_OPEN:
            Serial.println("** ERROR: No open file...");
            break;
          case FILE_IS_EMPTY:
            Serial.println("** ERROR: File is empty...");
            break;
          default:
            dumpBuffer(long(i)*512, result);
            i++;
            break;
        }              
      }
      Serial.println();
      file.closeFile();
    }
  }
  else
    Serial.println("** ERROR: 'FILE.DAT' does not exist...");
    
  Serial.println();
  Serial.println();
  Serial.println("***** All done... *****");
  
}  

void loop() 
{
}
