#ifndef FOUR_DEVICE_H
#define FOUR_DEVICE_H
#include <QString>

struct FourPortDevice //支持G E
{
public:
  int type; // G 0 E  1
  int nd1;
  int nd2;
  int ncd1;
  int ncd2;
  double value;   // c l仅仅记录值即可
  int branch_idx; //针对E
  QString name;
  double init_hz;
  FourPortDevice();
  ~FourPortDevice();
};
#endif // FOUR_DEVICE_H