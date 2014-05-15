// this code demonstrates the use of write_flash_page

#define write_flash_page (*((void(*)(prog_char * page, unsigned char * buffer))(0x7ffa/2)))

// use the second-to-last page in Flash, in Gamebuino the last one contains user settings
#define TARGET_PAGE ((prog_char *)(0x7000-256))

// set up a buffer to flash an entire page at a time
unsigned char buffer[128];

// accepts a value, writes it to flash, reads it back and prints it
void doTest(unsigned val)
{
  Serial.print("Flashing with 0x");
  Serial.print(val, HEX);
  *(unsigned *)buffer = val;
  write_flash_page (TARGET_PAGE, buffer);
  Serial.print("...read back value: 0x");
  Serial.println(pgm_read_word(TARGET_PAGE), HEX);
}

void setup() {
  Serial.begin(9600);
  
  // be very careful with code that reads a flash page, writes to it and then attempts to
  // read it back. even with judicious use of the volatile modifier the arduino compiler
  // seems to have a tendency to optimize out code that it shouldn't. to see this in action
  // try expanding the next two function calls out with hard-coded constants.
  doTest(0x1234);
  doTest(0x5678);
  
  Serial.println("Done!");
}

void loop() {
}
