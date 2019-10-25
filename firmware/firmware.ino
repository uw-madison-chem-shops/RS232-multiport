
// variables for USB serial communication
#define INPUT_SIZE 100
#define sep " "
char input[INPUT_SIZE + 1];
char *code;
char *c_index;
char *c_content;
int index = 0;
int number = 0;

// valve struct
typedef struct {
  int pinA;
  int pinB;
  char status=0;
} Valve;
Valve valves[6];

// pump serial handlers
#include <SoftwareSerial.h>
SoftwareSerial pump_1(11, 10);  // RX, TX
SoftwareSerial pump_2(16, 9);  // RX, TX
char from_pump;

void setup() {
  // valves
  valves[0].pinA = 2;
  valves[0].pinB = 4;
  valves[1].pinA = 5;
  valves[1].pinB = 6;
  valves[2].pinA = 8;
  valves[2].pinB = 7;
  valves[3].pinA = A4;
  valves[3].pinB = A5;
  valves[4].pinA = A2;
  valves[4].pinB = A3;
  valves[5].pinA = A0;
  valves[5].pinB = A1;
  for (int i=0; i <= 5; i++){
    pinMode(valves[i].pinA, OUTPUT);
    pinMode(valves[i].pinB, OUTPUT);
    digitalWrite(valves[i].pinA, HIGH);
    digitalWrite(valves[i].pinB, HIGH);
   }
  // serial to RS232
  pump_1.begin(9600);
  pump_2.begin(9600);
  // serial over USB
  Serial.begin(57600);
}

void loop() {
  while (pump_1.available()) {
    from_pump = pump_1.read();
    Serial.print(from_pump);
  }
  while (pump_2.available()) {
    from_pump = pump_2.read();
    Serial.print(from_pump);
  }
}

void serialEvent() {  // occurs whenever new data comes into the USB
  // read serial into input char array
  byte size = Serial.readBytesUntil('\n', input, INPUT_SIZE);
  input[size] = 0;
  // parse input, dividing at seps
  code = strtok(input, sep);
  c_index = strtok(0, sep);
  c_content = strtok(0, '\n');
  // coerce index to integer
  index = atoi(c_index);
  // process command
  if (*code == 'V') {
    number = atoi( c_content);
    index -= 1;
    int pins[2] = {valves[index].pinA, valves[index].pinB};
    digitalWrite(pins[number], LOW);
    delay(30);
    digitalWrite(valves[index].pinA, HIGH);
    digitalWrite(valves[index].pinB, HIGH);
    valves[index].status = (char) number;
  }
  else if (*code == 'P') {
    pump_1.flush();
    pump_2.flush();
    if (index == 1) {
      pump_1.listen();
      pump_1.write(c_content);
      pump_1.write('\r');
    }
    else if (index == 2) {
      pump_2.listen();
      pump_2.write(c_content);
      pump_2.write('\r');
    }
    else {
      pump_1.listen();
      pump_1.write(c_content);
      pump_1.write('\r');
      pump_1.listen();
      pump_2.write(c_content);
      pump_2.write('\r');
    }
    
  }
}

void serialEventRun(void) {
  if (Serial.available()) serialEvent();
}
