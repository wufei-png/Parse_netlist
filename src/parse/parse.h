#ifndef PARSE_H
#define PARSE_H
#include <QString>
#include <QDebug>
#include <QObject>
#include <QStringList>
#include <QRegExp>
// #include<QFileDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <vector>
#include <QApplication>
#include <unordered_map>
#include "devices/two_port_device.h"
#include "devices/four_port_device.h"
#include "devices/pulse_device.h"
#include <cmath>
#include "util/tool.h"
using namespace std;
const QString title = QString("Title: ");
const QString anno = QString("Annotation: ");
const QString device = QString("Device Type: ");
const QString analy = QString("Analysis Command: ");
const QString token = QString("token");
const QString unknow = QString("unknowen synax: ");
const QString unknow_print = QString("unknow");
const QString end_str = QString("end token");
const QString reg = QString(" Regsistor ");
const QString I = QString("I Source ");
const QString Vol = QString(" Voltage Source ");
const QString cap = QString(" Capacitor ");
const QString out_info = QString("  Output Command Print (Type:");
const QString L = QString("Inductance");
class circuit;
struct file_parse
{
private:
  QFile *file;
  bool end;
  int parsed_nums; //已经解析过的行数
  int empty_nums;  //空行数，将已经解析过的行数-空行数是否是1来判断这是不是第一个有效的行，为了判断是不是标题
  int Device_nums;
  int Node_nums;
  int V_nums;
  int R_nums;
  int L_nums;
  int C_nums;
  int I_nums;
  unordered_map<string, int> dev_map;
  unordered_map<string, int> node_map;
  QString get_cur_line();
  QString dev_info(QStringList);
  bool check_dev_name(QString &line, QString &res, bool simulation);
  bool check_dev_node_value(QStringList list, QString &res, bool simulation, unordered_map<string, int> &node2row); // sim参数代表不是dc模拟的 前两个是节点号，不应该含k或者小数点
  bool check_dev_form(QStringList &strlist, QString &res, bool, vector<double> &, bool &is_two, unordered_map<string, int> &node2row, double &init_hz);
  void check_print(const QString &str1, const QString &str2, QString &res);

public:
  QString parsed_res;
  file_parse(QFile *file1);
  ~file_parse();
  void show_dev_nums(QString &);
  QString get_next_line();
  QString parse_one_line(QString line, vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran);
  QString get_and_parse_one_line(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran);
  QString parse(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &sum_sz, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran, circuit *circuit);
  int get_dev_num();  //为了给矩阵提前分配空间
  int get_node_num(); //为了给矩阵提前分配空间
};
#endif // PARSE_H