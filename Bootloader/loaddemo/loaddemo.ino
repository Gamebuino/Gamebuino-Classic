// this code demonstrates the use of load_game
// upload it into gamebuino and then open the serial port monitor, you should see
// the "Send a char to start..." message. send a character to it and it
// will attempt to load and burn BLINK.HEX from the SD card

#define load_game (*((void(*)(const char* filename))(0x7ffc/2)))

void setup() {
  Serial.begin(9600);
  Serial.println("Send a char to start...");
}

void loop() {
  if (Serial.available()) {
    Serial.read();
    load_game("BLINK");
  }
}

