#ifndef ODOM_H
#define ODOM_H

class ODOM
{
private:
  const int pulsoPorVolta = 20;
  const double PI = 3.1415926535897932384626433832795;

  const double raio = 3.4;     // Em cm.
  const double disEixo = 7.5;  // Em cm.
  const double diametro = 6.8; // Em cm.

  double odom[3];
  int calcTeta(int, int);
  int calcX(int, int);
  int calcY(int, int);

public:
  ODOM(double *);
  double[] calcOdom(int, int);
};
#endif // ODOM_H