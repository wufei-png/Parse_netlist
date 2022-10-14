#ifndef PARSE_H
#define PARSE_H
#include <QString>
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
using namespace std;
const QString title = QString("Title: ");
const QString anno = QString("Annotation: ");
const QString device = QString("Device Type: ");
const QString analy = QString("Analysis Command: ");
const QString token = QString("token");
const QString unknow = QString("unknowen synax: ");
const QString end_str = QString("end token");
const QString reg = QString(" Regsistor ");
const QString Vol = QString(" Voltage Source ");
const QString cap = QString(" Capacitor ");
const QString out_info = QString("  Output Command Print (Type:");
const QString L = QString("Inductance");

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
  unordered_map<string, int> dev_map;
  unordered_map<string, int> node_map;
  QString get_cur_line();
  QString dev_info(QStringList);
  bool check_dev_name(QString &line, QString &res, bool simulation);
  bool check_dev_value(QStringList list, bool simulation); // sim参数代表不是dc模拟的 前两个是节点号，不应该含k或者小数点
  bool check_dev_form(QStringList &strlist, QString &res, bool sim);

public:
  file_parse(QFile *file1);
  ~file_parse();
  void show_dev_nums(QString &);
  QString get_next_line();
  QString parse_one_line(const QString line);
  QString get_and_parse_one_line();
  QString parse();
};
#endif // PARSE_H