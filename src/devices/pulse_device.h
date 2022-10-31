#ifndef PULSE_DEVICE_H
#define PULSE_DEVICE_H
#include <QString>
#include <QDebug>
#include <iostream>

using namespace std;
struct PulseDevice
{
public:
  int type; // v i
  int nd1;
  int nd2;
  double start_value; // c l仅仅记录值即可
  double pulse_value;
  double start_time;
  double tr;
  double tf;
  double pw;
  double per;
  int branch_idx; //针对V
  QString name;
  void show_info();
  PulseDevice();
  ~PulseDevice();
  double get_value(double time);
};
#endif // PULSE_DEVICE_H