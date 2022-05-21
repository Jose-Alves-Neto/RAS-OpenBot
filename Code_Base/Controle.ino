// C++ code
//

#include <PID_MOTOR.h>
#include <ODOM.h>

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

// PID Esq
double
    kpe = 0.238,
    kie = 0.862,
    kde = 0.05445;

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
double pos[3];
float velocidadeAngular = 0;
float velocidadeLinear = 2;
float disEixo = 7.5;

void setup()
{
  Serial.begin(9600);

  motorSetup();

  encodeSetup();

  calcDesiredRpm();
  librarySetup();
}

void calcDesiredRpm()
{
  desiredRpmDir = (velocidadeLinear * 100 + (disEixo * velocidadeAngular)) / (diametro / 2);
  desiredRpmEsq = (velocidadeLinear * 100 - (disEixo * velocidadeAngular)) / (diametro / 2);
}

void librarySetup()
{
  odom = new ODOM(pos);
  pidDir = new PID_MOTOR(rpmDir, velDir, kpd, kid, kdd, desiredRpmDir, updateTime);
  pidEsq = new PID_MOTOR(rpmEsq, velEsq, kpe, kie, kde, desiredRpmEsq, updateTime);
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

    calcPos(pulsoDir, pulsoEsq);
    printState();

    pulsoDir = 0;
    pulsoEsq = 0;

    timeOld = millis();

    interrupts();
  }
  pidDir.compute();
  pidEsq.compute();
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

void calcPos(pulsoDir, pulsoEsq)
{
  odom.calcOdom(pulsoDir, pulsoEsq);
}

void direcao()
{
  desiredRpmDir > 0 ? (digitalWrite(motorDirA, HIGH),
                       digitalWrite(motorDirB, LOW))
                    : (digitalWrite(motorDirA, LOW),
                       digitalWrite(motorDirB, HIGH));
  desiredRpmEsq > 0 ? (digitalWrite(motorEsqA, HIGH),
                       digitalWrite(motorEsqB, LOW))
                    : (digitalWrite(motorEsqA, LOW),
                       digitalWrite(motorEsqB, HIGH));
}

double calcDis(int pulso)
{
  return (PI * diametro * pulso) / pulsoPorVolta;
}

int calcRpm(int pulso)
{
  return (60L * 1000 / pulsoPorVolta) / (millis() - timeOld) * pulso;
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