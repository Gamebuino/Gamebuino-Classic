// SpeedCheck_writeLn (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program will measure the average time a call to
// the writeLn()-function takes.
//
// SD card must be connected to the SPI port of your Arduino.
//
// Remember to select 115200 baud in the Serial Monitor.
//

#include <tinyFAT.h>
#include <avr/pgmspace.h>

byte res;
long msStart, msEnd, msTotal, msAvg, msHigh, msLow;

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

  Serial.println("This program will measure the average time a call to the writeLn()-function takes.");
  Serial.println();
  Serial.println("***** Send any character to start *****");
  while (!Serial.available()) {};
  Serial.flush();
  
  if (file.exists("SPDCHK.FIL"))
    file.delFile("SPDCHK.FIL");
    
  file.create("SPDCHK.FIL");
  
  res=file.openFile("SPDCHK.FIL", FILEMODE_TEXT_WRITE);
  if (res==NO_ERROR)
  {
    msTotal=0;
    msLow=1000000;
    msHigh=0;
    
    Serial.print("<");
    for (int cc=0; cc<64; cc++)
    {
      if (((cc+1)%16)!=0)
        Serial.print("-");
      else
        Serial.print("+");
    }    
    Serial.println(">");
    
    Serial.print(" ");
    for (int cc=0; cc<1024; cc++)
    {
      msStart=millis();
      file.writeLn("SpeedCheck - This is just a random line of text.");
      msEnd=millis();

      msTotal+=(msEnd-msStart);
      if ((msEnd-msStart)<msLow)
        msLow=msEnd-msStart;
      if ((msEnd-msStart)>msHigh)
        msHigh=msEnd-msStart;
        
      if (((cc+1)%256)==0)
        Serial.print("+");
      else if (((cc+1)%16)==0)
        Serial.print("-");
    }
    file.closeFile();
    file.delFile("SPDCHK.FIL");
    Serial.println();
    Serial.println();
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
  Serial.println("Sample size : 1024 samples");
  Serial.print  ("Average time: ");
  Serial.print  (msTotal/1024, DEC);
  Serial.println(" ms");
  Serial.print  ("Highest time: ");
  Serial.print  (msHigh, DEC);
  Serial.println(" ms");
  Serial.print  ("Lowest time : ");
  Serial.print  (msLow, DEC);
  Serial.println(" ms");
  Serial.println();
  Serial.println("***** All done... *****");
  
}  

void loop() 
{
}
