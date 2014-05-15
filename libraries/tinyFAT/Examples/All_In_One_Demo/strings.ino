char strBuffer[60];

prog_char prompt_string_0[] PROGMEM = "** SD ready> ";

PROGMEM const char *prompt_string_table[] =   
{   
  prompt_string_0
};

prog_char help_string_0[] PROGMEM = "tinyFAT library ALL-In-One demo...";
prog_char help_string_1[] PROGMEM = "-----------------------";
prog_char help_string_2[] PROGMEM = "Help...";
prog_char help_string_3[] PROGMEM = "-------";
prog_char help_string_4[] PROGMEM = "? or H:\t\t\t\tThis help";
prog_char help_string_5[] PROGMEM = "I:\t\t\t\tInformation about partition and filesystem";
prog_char help_string_6[] PROGMEM = "L:\t\t\t\tList files on SD card";
prog_char help_string_7[] PROGMEM = "E <filename>:\t\t\tCheck if <filename> exists.";
prog_char help_string_8[] PROGMEM = "R <filename1> <filename1>:\tRename <filename1> to <filename2>.";
prog_char help_string_9[] PROGMEM = "D <filename>:\t\t\tDelete <filename>.";
prog_char help_string_10[] PROGMEM = "C <filename>:\t\t\tCreate an empty file named <filename>.";
prog_char help_string_11[] PROGMEM = "B <filename>:\t\t\tView contents of <filename> (Binary-mode)";
prog_char help_string_12[] PROGMEM = "T <filename>:\t\t\tView contents of <filename> (Text-mode)";
prog_char help_string_13[] PROGMEM = "W <filename>:\t\t\tWrite a line of text to <filename> (Text-mode)";

PROGMEM const char *help_string_table[] =   
{   
  help_string_0,
  help_string_1,
  help_string_2,
  help_string_3,
  help_string_4,
  help_string_5,
  help_string_6,
  help_string_7,
  help_string_8,
  help_string_9,
  help_string_10,
  help_string_11,
  help_string_12,
  help_string_13
};

prog_char info_string_0[]  PROGMEM = "First partition Type............: ";
prog_char info_string_1[]  PROGMEM = "First partition starts at sector: ";
prog_char info_string_2[]  PROGMEM = "First partition size in sectors.: ";
prog_char info_string_3[]  PROGMEM = "Partition Size in MiB...........: ";
prog_char info_string_4[]  PROGMEM = "Sectors per Cluster.............: ";
prog_char info_string_5[]  PROGMEM = "Reserved Sector Count...........: ";
prog_char info_string_6[]  PROGMEM = "Number of FATs..................: ";
prog_char info_string_7[]  PROGMEM = "Max Root Entries................: ";
prog_char info_string_8[]  PROGMEM = "Total Sectors...................: ";
prog_char info_string_9[]  PROGMEM = "Sectors per FAT.................: ";
prog_char info_string_10[] PROGMEM = "Hidden Sectors..................: ";
prog_char info_string_11[] PROGMEM = "Partition Serial Number.........: ";
prog_char info_string_12[] PROGMEM = "First Sector of Root Directory..: ";

PROGMEM const char *info_string_table[] =   
{   
  info_string_0,
  info_string_1,
  info_string_2,
  info_string_3,
  info_string_4,
  info_string_5,
  info_string_6,
  info_string_7,
  info_string_8,
  info_string_9,
  info_string_10,
  info_string_11,
  info_string_12
};


