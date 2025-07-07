// Stepper Motor Only
const int stepPinX = 2;  // Platform A
const int dirPinX  = 5;
const int stepPinY = 3;  // Platform A
const int dirPinY  = 6;
const int stepPinZ = 4;  // Platform B
const int dirPinZ  = 7;
const int stepPinE = 12; // Platform B
const int dirPinE  = 13;

const uint8_t MICROSTEP = 1;
const long stepsPerRevolution = 200 * MICROSTEP;
const unsigned int stepDelay = 800;

void setup() {
  Serial.begin(9600);

  pinMode(stepPinX, OUTPUT); pinMode(dirPinX, OUTPUT);
  pinMode(stepPinY, OUTPUT); pinMode(dirPinY, OUTPUT);
  pinMode(stepPinZ, OUTPUT); pinMode(dirPinZ, OUTPUT);
  pinMode(stepPinE, OUTPUT); pinMode(dirPinE, OUTPUT);
}

void stepMotors(int stepPin1, int dirPin1, int stepPin2, int dirPin2, bool up, long revCount) {
  long steps = stepsPerRevolution * revCount;
  digitalWrite(dirPin1, up ? HIGH : LOW);
  digitalWrite(dirPin2, up ? HIGH : LOW);
  for (long i = 0; i < steps; i++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(500);
}

void processMotorData(String data) {
  int commaPos = data.indexOf(',');
  if (commaPos == -1) return;

  String cmd = data.substring(0, commaPos);
  String val = data.substring(commaPos + 1);

  if (cmd == "platA") {
    stepMotors(stepPinX, dirPinX, stepPinY, dirPinY, val == "1", 5);
  } else if (cmd == "platB") {
    stepMotors(stepPinZ, dirPinZ, stepPinE, dirPinE, val == "1", 5);
  }
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processMotorData(input);
  }
}
