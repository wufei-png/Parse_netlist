#ifndef TWO_DEVICE_H
#define TWO_DEVICE_H
#include <QString>
#include <iostream>

using namespace std;
struct TwoPortDevice
{
public:
  int type; // r c l v i 0-4
  int nd1;
  int nd2;
  double value; // c l仅仅记录值即可
  double init_hz;
  int branch_idx; //针对V
  QString name;

  TwoPortDevice(QString name, int type, int nd1, int nd2, double value);
  TwoPortDevice();
  ~TwoPortDevice();
};
#endif // TWO_DEVICE_H