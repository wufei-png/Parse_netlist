#include "four_port_device.h"

FourPortDevice::FourPortDevice()
{
  name = QString();
  type = -1;
  nd1 = 0;
  nd2 = 0;
  ncd1 = 0;
  ncd2 = 0;
  value = 0;
  branch_idx = 0;
  init_hz = 0;
}
FourPortDevice::~FourPortDevice()
{
}