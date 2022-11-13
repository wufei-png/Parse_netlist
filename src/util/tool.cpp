#include "tool.h"
int qstring2int(QString a) // src/tools
{
  return atoi(a.toStdString().c_str());
}
double qstring2double(QString a) // src/tools 解析 +- 123 123.2 12.2k 2e-10
{
  double tmp = atof(a.toStdString().c_str()); //+ -都可以直接处理
  QString end = QString(a[a.size() - 1]);
  QString end_3 = QString(a.mid(a.size() - 3, 3));
  // wf = qstring2double("1meg");
  qDebug() << end_3;
  // QStringList left_list = left.split(' ');
  // int sz = left_list.size();
  // cout << sz;
  // QRegExp rx("([RVrv][a-z]*\\d*)"); // matching digits
  // QString test("GND");
  if (end.contains(QRegExp("^[kpmu]$", Qt::CaseInsensitive)))
  {
    if (end == "k" || end == "K") //有没有更简洁的办法?可以支持更多的数位 https://baike.baidu.com/item/%E6%95%B0%E9%87%8F%E7%BA%A7/3289119 因此后续有需要u,m什么的直接添加即可
    {
      tmp *= 1000;
    }
    else if (end == "p" || end == "P")
    {
      tmp *= pow(10, -12);
    }
    else if (end == "m")
    {
      tmp *= pow(10, -3);
    }
    else if (end == "M")
    {
      tmp *= pow(10, 6);
    }
    else if (end == "u" || end == "U")
    {
      tmp *= pow(10, -6);
    }
  }
  else if (end_3.contains(QRegExp("^meg$", Qt::CaseInsensitive)))
  {
    tmp *= pow(10, 6);
  }
  return tmp;
}
QString int2qstring(int a)
{
  return QString(to_string(a).c_str());
}
string doubleToString(const double &val)
{
  char *chCode;
  chCode = new char[20];
  sprintf(chCode, "%.2lf", val); //改进：小数点位数动态变化  问题：电容值比较小，直接就忽略了，应该转换成其他形式
  string str(chCode);
  delete[] chCode;
  return str;
}
string convertDouble(double value)
{
  std::ostringstream o;
  if (!(o << value))
    return "";
  return o.str();
}
QString doubleToQString(const double &val)
{
  return QString(doubleToString(val).c_str());
}
string complex2string(complex<double> &elem)
{
  string left;
  string right;

  if (elem.real() != 0)
    left = convertDouble(elem.real());
  if (elem.imag() != 0)
    right = convertDouble(elem.imag());

  if (!left.empty() && !right.empty())
  {
    if (elem.imag() > 0)
      return left + "+" + right + "i";
    else
      return left + right + "i";
  }

  if (left.empty() && !right.empty())
    return right + "i";

  if (!left.empty() && right.empty())
    return left;

  else
    return "0";
}
QString complex2qstring(complex<double> elem)
{
  return QString(complex2string(elem).c_str());
}
QString get_empty(int n)
{
  QString res;
  double end = 1.2 * n; //????
  for (int i = 0; i < end; i++)
    res += QString(" "); //不知道为什么对不齐
  return res;
}
int find_idx_from_devlist(vector<TwoPortDevice> &dev_list, QString &a)
{
  int i = 0;
  int sz = dev_list.size();
  for (; i < sz; i++)
  {
    if (!dev_list[i].name.compare(a, Qt::CaseInsensitive))
      break;
  }
  // qDebug() << "I::::" << i;
  return i < sz ? i : -1;
}

bool icompare_pred(unsigned char a, unsigned char b)
{
  return std::tolower(a) == std::tolower(b);
}

bool check_match_case_insensitive(std::string const &a, std::string const &b)
{
  if (a.length() == b.length())
  {
    return std::equal(b.begin(), b.end(),
                      a.begin(), icompare_pred);
  }
  else
  {
    return false;
  }
}
double complex_norm(const complex<double> x, bool is_tran)
{
  // qDebug() << imag(x);
  if (is_tran)
    return real(x);
  else
    return sqrt(real(x) * real(x) + imag(x) * imag(x));
}

const char *qstring2char(QString &a)
{
  return a.toStdString().c_str();
}