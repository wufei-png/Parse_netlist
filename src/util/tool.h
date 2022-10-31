#ifndef TOOL_H
#define TOOL_H
#include <QString>

#include <QObject>
#include <QDebug>
#include <QRegExp>
// #include<QFileDialog>
#include "devices/two_port_device.h"
#include "devices/pulse_device.h"
#include "devices/pulse_device.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <sstream> // ostringstream 类型
//考虑到rcl类都比较类似，直接在类成员里使用枚举类型区分
// E7 4 0 1 2 13如何判断是电压控制的电流源
using namespace std;
#include "tool.h"
int qstring2int(QString a); // src/tools

double qstring2double(QString a); // src/tools 解析 +- 123 123.2 12.2k 2e-10

QString int2qstring(int a);
string convertDouble(double value);
string doubleToString(const double &val);
QString doubleToQString(const double &val);
string complex2string(complex<double> &elem);
QString complex2qstring(complex<double> elem);
QString get_empty(int n);
int find_idx_from_devlist(vector<TwoPortDevice> &dev_list, QString &a);

bool icompare_pred(unsigned char a, unsigned char b);
bool check_match_case_insensitive(std::string const &a, std::string const &b);
double complex_norm(const complex<double> x, bool);
template <class T>
QString print_item(T &x, string head);
template <class T>
QString print_item(T &x, string head)
{
  ostringstream os; // 2
  x.print(os, head);
  return QString(os.str().c_str());
}
#endif // TOOL_H