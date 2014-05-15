void hexPrintByte(byte val)
{
  Serial.print("0x");
  if (val < 0x10)
    Serial.print("0");
  Serial.print(val, HEX);
  Serial.print(" [");
  Serial.print(val, DEC);
  Serial.println("] ");
}

void hexPrintWord(word val)
{
  Serial.print("0x");
  if (val < 0x1000)
    Serial.print("0");
  if (val < 0x100)
    Serial.print("0");
  if (val < 0x10)
    Serial.print("0");
  Serial.print(val, HEX);
  Serial.print(" [");
  Serial.print(val, DEC);
  Serial.println("] ");
}

void hexPrintLong(unsigned long val)
{
  Serial.print("0x");
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
  Serial.print(" [");
  Serial.print(val, DEC);
  Serial.println("] ");
}

void printTime(byte h, byte m)
{
  if (h<10)
    Serial.print("0");
  Serial.print(h, DEC);
  Serial.print(":");
  if (m<10)
    Serial.print("0");
  Serial.print(m, DEC);
}

char uCase(char c)
{
  if ((c>='a') && (c<='z'))
    return (c-0x20);
  else
    return c;
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


void printHelp(byte which)
{
  if (which==0x00)
  {
    strcpy_P(strBuffer, (char*)pgm_read_word(&(help_string_table[0])));
    Serial.println(strBuffer);
    strcpy_P(strBuffer, (char*)pgm_read_word(&(help_string_table[1])));
    Serial.println(strBuffer);
  }
  else
  {
    strcpy_P(strBuffer, (char*)pgm_read_word(&(help_string_table[2])));
    Serial.println(strBuffer);
    strcpy_P(strBuffer, (char*)pgm_read_word(&(help_string_table[3])));
    Serial.println(strBuffer);
  }
  for (int i=4; i<(sizeof(help_string_table)/sizeof(help_string_table[0])); i++)
  {  
    strcpy_P(strBuffer, (char*)pgm_read_word(&(help_string_table[i])));
    Serial.println(strBuffer);
  }
  Serial.println();
}

void printPrompt()
{
  strcpy_P(strBuffer, (char*)pgm_read_word(&(prompt_string_table[0])));
  Serial.print(strBuffer);
}

void printInfoStr(byte idx)
{
  strcpy_P(strBuffer, (char*)pgm_read_word(&(info_string_table[idx])));
  Serial.print(strBuffer);
}

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


