int encoderA = 2;

int motorA = 8;
int motorB = 9;

volatile int count;
unsigned long timeOld;

void setup() {
  pinMode(encoderA, INPUT);

  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(encoderA), count, RISING);
}

void loop() {
  if (count >= 20) {
    timeOld = millis();
    count = 0;
  }
}

void count() {
  count++;
}
