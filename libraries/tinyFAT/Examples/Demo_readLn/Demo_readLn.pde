// Demo_readLn (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of the readLn()-function.
//
// This demo will open a textfile named 'TEXTFILE.TXT'
// and display its contents in the Serial Monitor.
//
// SD card must be connected to the SPI port of your Arduino.
//
// Remember to select 115200 baud in the Serial Monitor.
//

#include <tinyFAT.h>
#include <avr/pgmspace.h>

byte res;
word result;
char textBuffer[81];

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

  Serial.println("This demo will display a textfile called 'TEXTFILE.TXT'");
  Serial.println();
  Serial.println("***** Send any character to start *****");
  while (!Serial.available()) {};
  Serial.flush();
  Serial.println();
  Serial.println();
  
  if (file.exists("TEXTFILE.TXT"))
  {  
    res=file.openFile("TEXTFILE.TXT", FILEMODE_TEXT_READ);
    if (res==NO_ERROR)
    {
      result=0;
      while ((result!=EOF) and (result!=FILE_IS_EMPTY))
      {
        result=file.readLn(textBuffer, 80);
        if (result!=FILE_IS_EMPTY)
        {
          if (result==BUFFER_OVERFLOW)
            Serial.print(textBuffer);
          else
            Serial.println(textBuffer);
        }
        else
          Serial.println("** ERROR: File is empty...");
      }
      Serial.println();
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
  }
  else
    Serial.println("** ERROR: 'TEXTFILE.TXT' does not exist...");
    
  Serial.println();
  Serial.println();
  Serial.println("***** All done... *****");
  
}  

void loop() 
{
}
