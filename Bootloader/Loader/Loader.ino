
#define load_game (*((void(*)(char*))0x7ffc))

void setup() {
  Serial.begin(9600);
  Serial.print(Press any key to load and launch BLINK...");  
}

void loop() {
  if (Serial.available())
    load_game("BLINK");
}
