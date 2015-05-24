// Demo_writeLn (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the writeLn()-function.
//
// This demo will create a textfile, and fill it with 1MB
// of text. The file will be named 'BIGFILE.TXT'. If the
// file already exists it will first be deleted.
//
// SD card must be connected to the SPI port of your Arduino.
//
// Remember to select 115200 baud in the Serial Monitor.
//

#include <tinyFAT.h>
#include <avr/pgmspace.h>

byte res;

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

  Serial.println("This demo will create a textfile, and fill it with 1MB of text.");
  Serial.println("The file will be named 'BIGFILE.TXT'. If the file already exists it will first be deleted.");
  Serial.println();
  Serial.println("***** Send any character to start *****");
  while (!Serial.available()) {};
  Serial.flush();
  
  if (file.exists("BIGFILE.TXT"))
    file.delFile("BIGFILE.TXT");
    
  file.create("BIGFILE.TXT");
  
  res=file.openFile("BIGFILE.TXT", FILEMODE_TEXT_WRITE);
  if (res==NO_ERROR)
  {
    for (int cc=0; cc<1024; cc++)
    {
      for (int i=0; i<8; i++)
      {
        file.writeLn("012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345");
      }
      if (((cc+1)%16)!=0)
        Serial.print(".");
      else
        Serial.print("|");
     if (((cc+1)%64)==0)
     {
       Serial.print(" ");
       Serial.print(cc+1, DEC);
       Serial.println(" KB");
     }
    }
    file.closeFile();
  }
  else
  {
    switch(res)
    {
      case ERROR_ANOTHER_FILE_OPEN:
        Serial.println("** ERROR: Another file is already open...");
        break;
      default:
        Serial.print("** ERROR: ");
        Serial.println(res, HEX);
        break;
    }
  }
  Serial.println("***** All done... *****");
  
}  

void loop() 
{
}
