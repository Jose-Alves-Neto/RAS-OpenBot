#ifndef PID_MOTOR_H
#define PID_MOTOR_H

class PID_MOTOR
{
private:
  double kp, ki, kd;
  double p, i, d;
  double error;
  unsigned long lastTime;

  double *input;
  double *output;
  double *setpoint;
  int updateTime;

public:
  PID_MOTOR(double *, double *, double, double, double, double *, int);

  bool compute(void);
};

#endif // PID_MOTOR_H