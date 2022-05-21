void PID_MOTOR::PID_MOTOR(double *input, double *output, double kp, double ki, double kd, double *setPoint, int updateTime)
{
  this->input = &input;
  this->output = &output;
  this->setpoint = &setPoint;
  this->updateTime = updateTime;

  this->kp = kp;
  this->ki = ki;
  this->kd = kd;

  this->p = 0;
  this->i = 0;
  this->d = 0;

  this->error = 0;
  this->lastTime = millis();
}

bool PID_MOTOR::compute()
{
  unsigned long now = millis();
  unsigned long dt = now - lastTime;
  if (dt < updateTime)
    return false;

  double error = *setpoint - *input;
  double d = (error - this->error) / dt;
  this->error = error;

  this->p = error * kp;
  this->i += error * ki * dt;
  this->d = d * kd;

  *output = this->p + this->i + this->d;

  if (output > 255)
    *output = 255;
  else if (output < 0)
    *output = 0;

  lastTime = now;
  return true;
}