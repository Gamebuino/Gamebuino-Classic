// All_In_One_Demo (C)2011 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of most of tinyFATs functions.
//
// SD card must be connected to the SPI port of your Arduino.
//
// Remember to select 115200 baud in the Serial Monitor.
//

#include <tinyFAT.h>
#include <avr/pgmspace.h>

byte initres;
byte res;
byte input;
char sFilename[13];
char sFilename2[13];
char textBuffer[81];
int i;
word result;
boolean tmp;
char tmpC;

/********************** MAIN ROUTINES SECTION  BELOW **********************/

void setup() {
// Initialize serial communication 
  Serial.begin(115200);
  Serial.println();
  printHelp (0x00);
// Initialize tinyFAT 
  initres=file.initFAT();
  if (initres!=NO_ERROR)
  {
    Serial.print("***** ERROR: ");
    Serial.println(verboseError(initres));
    while (true) {};
  }
  printPrompt();
}  

void loop() 
{
  while (!Serial.available()) {};
  input = uCase(Serial.read());
  
  Serial.println(char(input));
  switch (input)
  {
// Send some helpful information over the serial-link
    case '?':
    case 'H':
      printHelp(0x01);
      break;

// Display data from the Master Boot Record and Boot Sector
    case 'I':
      // MBR
      printInfoStr(0); hexPrintByte(file.MBR.part1Type);
      printInfoStr(1); hexPrintLong(file.MBR.part1Start);
      printInfoStr(2); hexPrintLong(file.MBR.part1Size);
      Serial.println();
      
      // Boot sector 
      printInfoStr(3);  Serial.println(file.BS.partitionSize);
      printInfoStr(4);  hexPrintByte(file.BS.sectorsPerCluster);
      printInfoStr(5);  hexPrintWord(file.BS.reservedSectors);
      printInfoStr(6);  hexPrintByte(file.BS.fatCopies);
      printInfoStr(7);  hexPrintWord(file.BS.rootDirectoryEntries);
      printInfoStr(8);  hexPrintLong(file.BS.totalFilesystemSectors);
      printInfoStr(9);  hexPrintWord(file.BS.sectorsPerFAT);
      printInfoStr(10); hexPrintLong(file.BS.hiddenSectors);
      printInfoStr(11); hexPrintLong(file.BS.partitionSerialNum);
      printInfoStr(12); hexPrintLong(file.firstDirSector);
      Serial.println();
      break;

// List all files on the SD card
    case 'L':
      Serial.println("Listing Files...");
      res = file.findFirstFile(&file.DE);
      if (res==NO_ERROR)
      {
        Serial.print(file.DE.filename);
        Serial.print(".");
        Serial.print(file.DE.fileext);
        Serial.print("   ");
        printTime(file.DE.time>>11, ((file.DE.time>>5) & B00111111));
        Serial.print("   Size: ");
        hexPrintLong(file.DE.fileSize);
      }
      else
      {
        Serial.println("No files found...");
      }
      while (res==NO_ERROR)
      {
        res = file.findNextFile(&file.DE);
        if (res==NO_ERROR)
        {
          Serial.print(file.DE.filename);
          Serial.print(".");
          Serial.print(file.DE.fileext);
          Serial.print("   ");
          printTime(file.DE.time>>11, ((file.DE.time>>5) & B00111111));
          Serial.print("   Size: ");
          hexPrintLong(file.DE.fileSize);
        }
      }
      Serial.println();
      break;

// Check if a file exists
    case 'E':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include a filename...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          while(Serial.available())
          {
            sFilename[i] = Serial.read();
            i++;
          }
          sFilename[i] = '\0';

          Serial.flush();

          if (file.exists(sFilename))
          {
            Serial.print(sFilename);
            Serial.println(" exists...");
          }
          else
          {
            Serial.print(sFilename);
            Serial.println(" does NOT exist...");
          }
          Serial.println();
        }
      }
      break;

// Rename a file
    case 'R':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include filenames...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          tmp=false;
          while(Serial.available())
          {
            tmpC=Serial.read();
            if (tmpC==0x20)
            {
              sFilename[i] = '\0';
              tmp=true;
              i=0;
            }
            else if (tmp==false)
            {
              sFilename[i] = tmpC;
              i++;
            }
            else
            {
              sFilename2[i] = tmpC;
              i++;
            }
          }
          sFilename2[i] = '\0';
          if (tmp==false)
          {
            Serial.println("** ERROR: Please include a second filename...");
            Serial.println();
            break;
          }
          Serial.print("Rename ");
          Serial.print(sFilename);
          Serial.print(" to ");
          Serial.println(sFilename2);

          Serial.flush();
          
          if (file.rename(sFilename, sFilename2))
            Serial.println("OK");
          else
            Serial.println("ERROR");
          Serial.println();
        }
      }
      break;

// Delete a file
    case 'D':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include a filename...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          while(Serial.available())
          {
            sFilename[i] = Serial.read();
            i++;
          }
          sFilename[i] = '\0';

          Serial.flush();

          if (file.delFile(sFilename))
          {
            Serial.print(sFilename);
            Serial.println(" deleted...");
          }
          else
          {
            Serial.print(sFilename);
            Serial.println(" NOT deleted...");
          }
          Serial.println();
        }
      }
      break;

// Create a new, empty file
    case 'C':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include a filename...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          while(Serial.available())
          {
            sFilename[i] = Serial.read();
            i++;
          }
          sFilename[i] = '\0';

          Serial.flush();

          if (file.create(sFilename))
          {
            Serial.print(sFilename);
            Serial.println(" created...");
          }
          else
          {
            Serial.print(sFilename);
            Serial.println(" NOT created...");
          }
          Serial.println();
        }
      }
      break;

// Perform a binary dump of a file
    case 'B':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include a filename...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          while(Serial.available())
          {
            sFilename[i] = uCase(Serial.read());
            i++;
          }
          sFilename[i] = '\0';

          Serial.flush();
          i=0;
          res=file.openFile(sFilename, FILEMODE_BINARY);
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
                default:
                  dumpBuffer(long(i*512), result);
                  i++;
                  break;
              }              
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
              case ERROR_FILE_NOT_FOUND:
                Serial.println("** ERROR: File not found...");
                break;
            }
          }
          Serial.println();
        }
      }
      break;

// Perform a dump of a text-file
    case 'T':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include a filename...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          while(Serial.available())
          {
            sFilename[i] = Serial.read();
            i++;
          }
          sFilename[i] = '\0';

          Serial.flush();
          i=0;
          res=file.openFile(sFilename, FILEMODE_TEXT_READ);
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
              case ERROR_FILE_NOT_FOUND:
                Serial.println("** ERROR: File not found...");
                break;
            }
          }
          Serial.println();
        }
      }
      break;

// Write a line of text at the end of a text-file
    case 'W':
      delay(10);
      if (!Serial.available())
      {
        Serial.println("** ERROR: Please include a filename...");
        Serial.println();
      }
      else
      {
        input = Serial.read();
        if (input!=0x20)
        {
          Serial.println("** ERROR: Please separate command and filename with a <space>...");
          Serial.println();
        }
        else
        {
          delay(10);
          i=0;
          while(Serial.available())
          {
            sFilename[i] = Serial.read();
            i++;
          }
          sFilename[i] = '\0';

          Serial.flush();
          i=0;
          res=file.openFile(sFilename, FILEMODE_TEXT_WRITE);
          if (res==NO_ERROR)
          {
            file.writeLn("This is just a line of silly text. NI! NI! NI!");
            file.closeFile();
          }
          else
          {
            switch(res)
            {
              case ERROR_ANOTHER_FILE_OPEN:
                Serial.println("** ERROR: Another file is already open...");
                break;
              case ERROR_FILE_NOT_FOUND:
                Serial.println("** ERROR: File not found...");
                break;
            }
          }
          Serial.println();
        }
      }
      break;
  }
  Serial.flush();
  printPrompt();
}

