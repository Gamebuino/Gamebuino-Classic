#include <petit_fatfs.h>

void strobe(int pin)
{
  /*digitalWrite(pin, HIGH);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);*/
}

void setup()
{
  strobe(3);
  
  char * s = (char*)calloc(65, sizeof(char));
  delay(500);
  spi_init();
  Serial.begin(115200);
  
  PFFS.begin(2, rx, tx);
  
  fileReadTest(PFFS.open_file("test.txt"), "test.txt");
  
  FILINFO fnfo;
  
  do
  {    
    dirReadTest(PFFS.read_dir(&fnfo), &fnfo);
  }
  while (fnfo.fname[0] != 0);
  
  Serial.print("Dir Rewind, err = "); Serial.print(PFFS.rewind_dir());
  
  do
  {
    PFFS.read_dir(&fnfo);
  }
  while (fnfo.fattrib & AM_DIR == 0 && fnfo.fname[0] != 0);
  
  Serial.print("Opening directory: "); Serial.println(fnfo.fname);
  
  int err = PFFS.open(&fnfo);
  
  if (err == 0)
  {
    do
    {    
      dirReadTest(PFFS.read_dir(&fnfo), &fnfo);
    }
    while (fnfo.fname[0] != 0);
    
    Serial.print("Dir Rewind, err = "); Serial.println(PFFS.rewind_dir());
  }
  else
  {
    Serial.print("Open dir failed, err = "); Serial.println(err);
  }
  
  Serial.print("Up one dir, err = "); Serial.println(PFFS.up_dir());
  
  if (err == 0)
  {
    do
    {    
      dirReadTest(PFFS.read_dir(&fnfo), &fnfo);
    }
    while (fnfo.fname[0] != 0);
  }
}

void dirReadTest(int err, FILINFO * fnfo)
{
  Serial.print("Read Dir, err = "); Serial.println(err);
  if (err == 0)
  {
    Serial.print("Name: "); Serial.print(fnfo->fname);
    if (fnfo->fattrib & AM_DIR)
    {
      Serial.println(" is a directory");
    }
    else
    {
      Serial.println(" is a file");
      fileReadTest(PFFS.open(fnfo), fnfo->fname);
    }
  }
}

void fileReadTest(int err, char * fp)
{
  if (err == 0)
  {  
    Serial.print("Reading File "); Serial.print(fp); Serial.print(" (err = "); Serial.print(err); Serial.println(" == 0) :");
    
    char * s = (char*)calloc(65, sizeof(char));
    int bytes_read;
    int bytes_cnt = 0;
    
    do
    {
      PFFS.read_file(s, 64, &bytes_read);
      bytes_cnt += bytes_read;
      Serial.print(s);
    }
    while (bytes_read == 64);

    free(s);
    
    Serial.println();
    Serial.print("Read "); Serial.print(bytes_cnt); Serial.println(" bytes");
  }
  else
  {
    Serial.print("Error code "); Serial.print(err); Serial.print(" while opening "); Serial.println(fp);
  }
}

void loop()
{
  
}

byte rx()
{
  SPDR = 0xFF;
  loop_until_bit_is_set(SPSR, SPIF);
  return SPDR;
}

void tx(byte d)
{
  SPDR = d;
  loop_until_bit_is_set(SPSR, SPIF);
}

void spi_init()
{
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
  
  SPCR = _BV(MSTR) | _BV(SPE);      // Master mode, SPI enable, clock speed MCU_XTAL/4
}
