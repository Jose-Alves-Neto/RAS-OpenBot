// C++ code
//

//Arduino
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
int updateTime = 800;
// PID Dir

double
  kpd = 0.1371,
  kid = 0.000931,
  kdd = 0.0000000001;
 
double
  Pd = 0,
  Id = 0,
  Dd = 0;

double errorDir;
unsigned long lastTimeDir = 0;

// PID Esq
double
  kpe = 0.0993,
  kie = 0.000889,
  kde = 0.0000000014;
 
double
  Pe = 0,
  Ie = 0,
  De = 0;

double errorEsq;
unsigned long lastTimeEsq = 0;

//TEMPOS

volatile byte pulsoDir;
volatile byte pulsoEsq;

volatile int pulsoPosDir;
volatile int pulsoPosEsq;
int lastPulsoPosDir;
int lastPulsoPosEsq;

unsigned long timeOld = 0;
unsigned long timeOldDir = 0;
unsigned long timeOldEsq = 0;

unsigned long startFix = 0;

int velDir;
int rpmDir;
int lastRpmDir;

int velEsq;
int rpmEsq;
int lastRpmEsq;

boolean stopMoving = false;
boolean stopAng = false;

boolean sentidoDir = false;
boolean sentidoEsq = false;

const int pulsoPorVolta = 20;

//CONSTANTES
#define PI 3.1415926535897932384626433832795
float diametro = 6.8;
float raio = 3.4;
float disDireita;
float disEsquerda;

//MODELO CINEMATICO
float x = 0, y = 0, teta = 0;
float velocidadeAngular = 60;
float velocidadeLinear = 2;
float disEixo = 7.5;

//POSICAO FINAL
float dx = 0, dy = 0, dteta = 0;

void setup()
{
  Serial.begin(9600);
 
  motorSetup();

  encodeSetup();

  startFix = millis();
}

void calcDesiredRpm(int velocidadeLin, int velocidadeAng) {
  desiredRpmDir = (velocidadeLin * 100 + (disEixo * velocidadeAng)) / (diametro / 2);
  desiredRpmEsq = (velocidadeLin * 100 - (disEixo * velocidadeAng)) / (diametro / 2);
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
  moveAngulo(80);
 
  direcao();

  //Calcula o delta de pulso a cada interação
  int deltaPulsoPosDir = pulsoPosDir - lastPulsoPosDir;
  int deltaPulsoPosEsq = pulsoPosEsq - lastPulsoPosEsq;
  lastPulsoPosDir = pulsoPosDir;
  lastPulsoPosEsq = pulsoPosEsq;
 
  plotMotor();
  plotPos(deltaPulsoPosDir, deltaPulsoPosEsq);
 
  if (millis() < 100) {
    analogWrite(pwmDir, 90);
    analogWrite(pwmEsq, 90);
    return;
  }

  else if (millis() - timeOld >= updateTime)
  {
    noInterrupts();

    lastRpmDir = rpmDir;
    rpmDir = calcRpm(pulsoDir);

    lastRpmEsq = rpmEsq;
    rpmEsq = calcRpm(pulsoEsq);

    pulsoDir = 0;
    pulsoEsq = 0;
   
    timeOld = millis();

    Serial.println(teta * (180 / PI));
   
    interrupts();
  }
 
  calcVelDir();
  calcVelEsq();
  analogWrite(pwmDir, velDir);
  analogWrite(pwmEsq, velEsq);
}

void plotPos(int pulsoDir, int pulsoEsq) {
  //Calcula a posição a cada interação
  calcTeta(pulsoDir, pulsoEsq);
  calcX(pulsoDir, pulsoEsq);
  calcY(pulsoDir, pulsoEsq);
 
  if (millis() - timeOld >= updateTime && !stopMoving) {
    Serial.print("Angulo: ");
    Serial.println(teta * (180 / PI));
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.println(y);
  }
}

void plotMotor() {
  if (millis() - timeOld >= updateTime && !stopMoving) {
    Serial.print("RPM Dir = ");
    Serial.print(rpmDir, DEC);
    Serial.print(" |   RPM Esq = ");
    Serial.println(rpmEsq, DEC);
    Serial.print(" Pwm Dir = ");
    Serial.print(velDir, DEC);
    Serial.print(" |   Pwm Esq = ");
    Serial.println(velEsq, DEC);
    Serial.println("---------");
  }
}

void moveFrente(double distancia) {
  double dist = sqrt(pow(x, 2) + pow(y, 2));
  if(dist > distancia) {
    stopMoving = true;
    return;
  }
 
  if (dist < distancia) {
    calcDesiredRpm(velocidadeLinear, 0);
  }
}

void moveAngulo(double angulo) {
  float c = 2 * PI * disEixo; // em cm
  float timeToTurn = (((angulo * PI) / 180) * c) / (2 * PI * raio * velocidadeLinear) * 1000;

  if(millis() > startFix + timeToTurn + 320) {
    stopMoving = true;
    return;
  }
 
  if (millis() - startFix >= timeToTurn) {
    calcDesiredRpm(0, velocidadeAngular);
  }
}

void direcao() {
  if (!stopMoving) {
    if (desiredRpmDir > 0) {
      digitalWrite(motorDirA,HIGH);
      digitalWrite(motorDirB,LOW);
      sentidoDir = true;
    } else {
      digitalWrite(motorDirA,LOW);
      digitalWrite(motorDirB,HIGH);
      sentidoDir = false;
    }

    if (desiredRpmEsq > 0) {
      digitalWrite(motorEsqA,HIGH);
      digitalWrite(motorEsqB,LOW);
      sentidoEsq = true;
    } else {
      digitalWrite(motorEsqA,LOW);
      digitalWrite(motorEsqB,HIGH);
      sentidoEsq = false;
    }
  return;
  }
  pare();
}

void calcTeta(int pulsoDir, int pulsoEsq) {
  teta += ((calcDis(pulsoDir) - calcDis(pulsoEsq)) / (2 * disEixo));
}

void calcX(int pulsoDir, int pulsoEsq) {
  x += ((calcDis(pulsoDir) + calcDis(pulsoEsq)) / 2) * cos(teta);
}

void calcY(int pulsoDir, int pulsoEsq) {
  y += (((calcDis(pulsoDir) + calcDis(pulsoEsq)) / 2) * sin(teta));
}


double calcDis(int pulso) {
  return (PI * diametro * pulso) / pulsoPorVolta;
}

int calcRpm (int pulso) {
  return ((60L * 1000 * pulso) / pulsoPorVolta)/ (millis() - timeOld) ;
}

void calcVelDir() {
 
  unsigned long nowDir = millis();
  unsigned long timeChangeDir = (nowDir - lastTimeDir);

  if (timeChangeDir>=updateTime && (desiredRpmDir - lastRpmDir > 3 || desiredRpmDir - lastRpmDir < -3)){
    errorDir = abs(desiredRpmDir) - rpmDir;
    double dInput = (rpmDir - lastRpmDir) / timeChangeDir;
   
    Pd = errorDir * kpd;
    Id += errorDir * kid * timeChangeDir;
    Dd = dInput * kdd;
   
    velDir = Pd + Id + Dd;
    if (velDir > 255) velDir = 255;
    else if (velDir < 70) velDir = 70;
   
    lastTimeDir = nowDir;
  }
}

void calcVelEsq() {
 
  unsigned long nowEsq = millis();
  unsigned long timeChangeEsq = (nowEsq - lastTimeEsq);

  if (timeChangeEsq>=updateTime && (desiredRpmEsq - lastRpmEsq > 3 || desiredRpmEsq - lastRpmEsq < -3)){
    errorEsq = abs(desiredRpmEsq) - rpmEsq;
    double dInput = (rpmEsq - lastRpmEsq) / timeChangeEsq;
   
    Pe = errorEsq * kpe;
    Ie += errorEsq * kie * timeChangeEsq;
    De = dInput * kde;
   
    velEsq = Pe + Ie + De;
   
    if (velEsq > 255) velEsq = 255;
    else if (velEsq < 60) velEsq = 60;
 
    lastTimeEsq = nowEsq;
  }
}

void contadorDir()
{
  pulsoDir++;
  pulsoPosDir++;
}

void contadorEsq()
{
  pulsoEsq++;
  pulsoPosEsq++;
}


void frente() {
  digitalWrite(motorDirA,HIGH);
  digitalWrite(motorDirB,LOW);
  digitalWrite(motorEsqA,HIGH);
  digitalWrite(motorEsqB,LOW);
}

void reverso() {
  digitalWrite(motorDirB,HIGH);
  digitalWrite(motorDirA,LOW);
  digitalWrite(motorEsqB,HIGH);
  digitalWrite(motorEsqA,LOW);
}

void esquerda() {
  digitalWrite(motorDirA,LOW);
  digitalWrite(motorDirB,HIGH);
  digitalWrite(motorEsqA,HIGH);
  digitalWrite(motorEsqB,LOW);
}

void direita() {
  digitalWrite(motorDirB,HIGH);
  digitalWrite(motorDirA,LOW);
  digitalWrite(motorEsqB,LOW);
  digitalWrite(motorEsqA,HIGH);
}

void pare() {
  digitalWrite(motorDirB,LOW);
  digitalWrite(motorDirA,LOW);
  digitalWrite(motorEsqB,LOW);
  digitalWrite(motorEsqA,LOW);
}