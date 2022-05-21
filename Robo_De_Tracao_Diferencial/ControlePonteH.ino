// C++ code
//

int motorRP = 8;
int motorRN = 9;

int motorLP = 2;
int motorLN = 3;

int pwmDir = 5;
int pwmEsq = 6;

int vel1 = 200;
int vel2 = 200;

void setup()
{
  pinMode(motorRP, OUTPUT);
  pinMode(motorRN, OUTPUT);
  pinMode(motorLP, OUTPUT);
  pinMode(motorLN, OUTPUT);

  pinMode(pwmDir, OUTPUT);
  pinMode(pwmEsq, OUTPUT);
}

void loop()
{
  velocidade();

  frente();
  delay(5000);
  reverso();
  delay(5000);
}

void velocidade()
{
  analogWrite(pwmDir, vel1);
  analogWrite(pwmEsq, vel2);
}

void frente()
{
  digitalWrite(motorRP, HIGH);
  digitalWrite(motorRN, LOW);
  digitalWrite(motorLP, HIGH);
  digitalWrite(motorLN, LOW);
}

void reverso()
{
  digitalWrite(motorRP, LOW);
  digitalWrite(motorRN, HIGH);
  digitalWrite(motorLP, LOW);
  digitalWrite(motorLN, HIGH);
}

void esquerda()
{
  digitalWrite(motorRP, HIGH);
  digitalWrite(motorRN, LOW);
  digitalWrite(motorLP, LOW);
  digitalWrite(motorLN, HIGH);
}

void direita()
{
  digitalWrite(motorRP, LOW);
  digitalWrite(motorRN, HIGH);
  digitalWrite(motorLP, HIGH);
  digitalWrite(motorLN, LOW);
}