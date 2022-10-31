#include "two_port_device.h"
using namespace std;
TwoPortDevice::TwoPortDevice(QString name = NULL, int type = 0, int nd1 = 0, int nd2 = 0, double value = 0) : name(name), type(type), nd1(nd1), nd2(nd2), value(value)
{
}
TwoPortDevice::TwoPortDevice()
{
  name = QString();
  type = -1;
  nd1 = 0;
  nd2 = 0;
  value = 0;
  branch_idx = 0;
  init_hz = 0;
}
TwoPortDevice::~TwoPortDevice()
{
}
