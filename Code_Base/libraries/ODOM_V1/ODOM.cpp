ODOM::ODOM(double *odom)
{
  this->odom = odom;
}

double ODOM::calcTeta(int pulsoDir, int pulsoEsq)
{
  return ((2 * PI * raio * (pulsoDir - pulsoEsq)) / (pulsoPorVolta * 2 * disEixo));
}

double ODOM::calcX(int pulsoDir, int pulsoEsq)
{
  return ((PI * raio * (pulsoDir + pulsoEsq)) / pulsoPorVolta) * cos(teta);
}

double ODOM::calcY(int pulsoDir, int pulsoEsq)
{
  return ((PI * raio * (pulsoDir + pulsoEsq)) / pulsoPorVolta) * sin(teta);
}

void ODOM::calcOdom(int pulsoDir, int pulsoEsq)
{
  odom[0] += calcX(pulsoDir, pulsoEsq);
  odom[1] += calcY(pulsoDir, pulsoEsq);
  odom[2] += calcTeta(pulsoDir, pulsoEsq);
}