// C++ code
//

// Arduino
const int encoderDir = 2;
const int encoderEsq = 3;

const int motorDirB = 10;
const int motorDirA = 11;
const int motorEsqA = 12;
const int motorEsqB = 13;

const int pwmDir = 5;
const int pwmEsq = 6;

int desiredRpmDir;
int desiredRpmEsq;
int updateTime = 2000;
// PID Dir

double
    kpd = 0.216,
    kid = 0.644,
    kdd = 0.0837;

double
    Pd = 0,
    Id = 0,
    Dd = 0;

double errorDir;
unsigned long lastTimeDir = 0;

// PID Esq
double
    kpe = 0.238,
    kie = 0.862,
    kde = 0.05445;

double
    Pe = 0,
    Ie = 0,
    De = 0;

double errorEsq;
unsigned long lastTimeEsq = 0;

// TEMPOS

volatile byte pulsoDir;
volatile byte pulsoEsq;

unsigned long timeOld = 0;
unsigned long timeOldDir = 0;
unsigned long timeOldEsq = 0;

int velDir;
int rpmDir;
int lastRpmDir;

int velEsq;
int rpmEsq;
int lastRpmEsq;

const int pulsoPorVolta = 20;

#define PI 3.1415926535897932384626433832795
float diametro = 6.8;
float raio = 3.4;
float disDireita;
float disEsquerda;

// MODELO CINEMATICO
float x = 0, y = 0, teta = 0;
float velocidadeAngular = 0;
float velocidadeLinear = 2;
float disEixo = 7.5;

void setup()
{
  Serial.begin(9600);

  motorSetup();

  encodeSetup();

  calcDesiredRpm();
}

void calcDesiredRpm()
{
  desiredRpmDir = (velocidadeLinear * 100 + (disEixo * velocidadeAngular)) / (diametro / 2);
  desiredRpmEsq = (velocidadeLinear * 100 - (disEixo * velocidadeAngular)) / (diametro / 2);

  Serial.println(desiredRpmDir);
  Serial.println(desiredRpmEsq);
}

void motorSetup()
{
  pinMode(motorDirA, OUTPUT);
  pinMode(motorDirB, OUTPUT);
  pinMode(motorEsqA, OUTPUT);
  pinMode(motorEsqB, OUTPUT);

  pinMode(pwmDir, OUTPUT);
  pinMode(pwmEsq, OUTPUT);
}

void encodeSetup()
{
  pinMode(encoderDir, INPUT);
  pinMode(encoderEsq, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderDir), contadorDir, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderEsq), contadorEsq, RISING);

  pulsoDir = 0;
  pulsoEsq = 0;

  rpmDir = 0;
  rpmEsq = 0;

  timeOld = 0;
}

void loop()
{
  direcao();

  if (millis() - timeOld >= updateTime)
  {
    noInterrupts();

    lastRpmDir = rpmDir;
    rpmDir = calcRpm(pulsoDir);

    lastRpmEsq = rpmEsq;
    rpmEsq = calcRpm(pulsoEsq);

    disDireita += calcDis(pulsoDir);
    disEsquerda += calcDis(pulsoEsq);

    calcPos();
    printState();

    pulsoDir = 0;
    pulsoEsq = 0;

    timeOld = millis();

    interrupts();
  }
  calcVelDir();
  calcVelEsq();
  analogWrite(pwmDir, velDir);
  analogWrite(pwmEsq, velEsq);
}

void printState()
{
  Serial.println(teta);
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);

  Serial.print("RPM Dir = ");
  Serial.print(rpmDir, DEC);
  Serial.print(" |   RPM Esq = ");
  Serial.println(rpmEsq, DEC);
  Serial.print("Dis Dir: ");
  Serial.print(disDireita);
  Serial.print(" |   Dis Esq: ");
  Serial.println(disEsquerda);
}

void calcPos()
{
  calcTeta(pulsoDir, pulsoEsq);
  calcX(pulsoDir, pulsoEsq);
  calcY(pulsoDir, pulsoEsq);
}

void direcao()
{
  if (desiredRpmDir > 0)
  {
    digitalWrite(motorDirA, HIGH);
    digitalWrite(motorDirB, LOW);
  }
  else
  {
    digitalWrite(motorDirA, LOW);
    digitalWrite(motorDirB, HIGH);
  }

  if (desiredRpmEsq > 0)
  {
    digitalWrite(motorEsqA, HIGH);
    digitalWrite(motorEsqB, LOW);
  }
  else
  {
    digitalWrite(motorEsqA, LOW);
    digitalWrite(motorEsqB, HIGH);
  }
}

void calcTeta(int pulsoDir, int pulsoEsq)
{
  teta += ((2 * PI * raio * (pulsoDir - pulsoEsq)) / (pulsoPorVolta * 2 * disEixo));
}

void calcX(int pulsoDir, int pulsoEsq)
{
  x += ((PI * raio * (pulsoDir + pulsoEsq)) / pulsoPorVolta) * cos(teta);
}

void calcY(int pulsoDir, int pulsoEsq)
{
  y += ((PI * raio * (pulsoDir + pulsoEsq)) / pulsoPorVolta) * sin(teta);
}

double calcDis(int pulso)
{
  return (PI * diametro * pulso) / pulsoPorVolta;
}

int calcRpm(int pulso)
{
  return (60L * 1000 / pulsoPorVolta) / (millis() - timeOld) * pulso;
}

void calcVelDir()
{

  unsigned long nowDir = millis();
  unsigned long timeChangeDir = (nowDir - lastTimeDir);

  if (timeChangeDir >= updateTime)
  {
    errorDir = abs(desiredRpmDir) - rpmDir;
    double dInput = (rpmDir - lastRpmDir);

    Pd = errorDir * kpd;
    Id += errorDir * kid;
    Dd = dInput * kdd;

    velDir = Pd + Id - Dd;
    if (velDir > 255)
      velDir = 255;
    else if (velDir < 0)
      velDir = 0;

    lastTimeDir = nowDir;
  }
}

void calcVelEsq()
{

  unsigned long nowEsq = millis();
  unsigned long timeChangeEsq = (nowEsq - lastTimeEsq);

  if (timeChangeEsq >= updateTime)
  {
    errorEsq = abs(desiredRpmEsq) - rpmEsq;
    double dInput = (rpmEsq - lastRpmEsq);

    Pe = errorEsq * kpe;
    Ie += errorEsq * kie;
    De = dInput * kde;

    velEsq = Pe + Ie - De;

    if (velEsq > 255)
      velEsq = 255;
    else if (velEsq < 0)
      velEsq = 0;

    lastTimeEsq = nowEsq;
  }
}

void contadorDir()
{
  pulsoDir++;
}

void contadorEsq()
{
  pulsoEsq++;
}

void frente()
{
  digitalWrite(motorDirA, HIGH);
  digitalWrite(motorDirB, LOW);
  digitalWrite(motorEsqA, HIGH);
  digitalWrite(motorEsqB, LOW);
}

void reverso()
{
  digitalWrite(motorDirB, HIGH);
  digitalWrite(motorDirA, LOW);
  digitalWrite(motorEsqB, HIGH);
  digitalWrite(motorEsqA, LOW);
}

void pare()
{
  digitalWrite(motorDirB, LOW);
  digitalWrite(motorDirA, LOW);
  digitalWrite(motorEsqB, LOW);
  digitalWrite(motorEsqA, LOW);
}