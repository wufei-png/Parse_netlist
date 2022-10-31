#include "pulse_device.h"
using namespace std;

PulseDevice::PulseDevice()
{
  name = QString();
  type = -1;
  nd1 = 0;
  nd2 = 0;
  start_value = 0; // c l仅仅记录值即可
  pulse_value = 0;
  start_time = 0;
  tr = 0;
  tf = 0;
  pw = 0;
  per = 0;
  branch_idx = 0;
}
PulseDevice::~PulseDevice()
{
}
void PulseDevice::show_info()
{
  qDebug() << "name" << name;
  qDebug() << "type" << type;
  qDebug() << "nd1" << nd1;
  qDebug() << "nd2" << nd2;
  qDebug() << "start_value" << start_value;
  qDebug() << "pulse_value" << pulse_value;
  qDebug() << "start_time" << start_time;
  qDebug() << "tr" << tr;
  qDebug() << "tf" << tf;
  qDebug() << "pw" << pw;
  qDebug() << "per" << per;
}

double PulseDevice::get_value(double time)
{
  if (time <= start_time)
    return start_value;
  time -= start_time;

  if (time > per)
  {
    int per_num = time / per;
    time -= per_num * per;
  }

  double up_k = (pulse_value - start_value) / tr;
  double down_k = (pulse_value - start_value) / tf;

  if (time < tr)
    return start_value + time * up_k;

  if (time >= tr && time <= tr + pw)
    return pulse_value;

  if (time > tr + pw && time < tr + pw + tf)
    return pulse_value - (time - tr - pw) * down_k;

  if (time >= tr + pw + tf)
    return start_value;
}