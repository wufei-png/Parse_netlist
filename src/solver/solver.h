#ifndef SOLVER_H
#define SOLVER_H

#include <QString>
#include <QRegExp>
#include <QMessageBox>
// #include<QFileDialog>
#include <iostream>
#include <vector>
#include <complex>
#include <QDebug>
#include <QVector>
#include <cmath>
#include "devices/two_port_device.h"
#include "devices/four_port_device.h"
#include "devices/pulse_device.h"
#include "util/tool.h"
#include <unordered_map>
#include <armadillo> //V2
#include <sstream>   // ostringstream 类型
/*
使用稀疏矩阵的原因：
一个节点只跟相邻的两个节点有关系，比如电阻的节点为1，2，电路复杂有很多节点，那么在第1和第二行除了2和1列其他都为0，这很占空间。

// 关于hw3 testbench3,4（tran）的逻辑：首先tran的时候c,l的stamp和原先不同，所以我需要另外的一个二维矩阵来存储tran需要的。用同样的逻辑构造出这个矩阵后，就有了ppt这里的形式：
//ppt.jpg
// 其中LHS的时刻都是t时刻，是我要求的量，RHS中，除了vin是t时刻，其余的都是t-h。我用dc模拟的结果作为RHS的起始值(0时刻），然后解方程得到0+h时刻的量（LHS)，再把他们赋给RHS再解得到延后0+2h的数值。

*/
using namespace std;
using namespace arma; //??????很重要，不然报错
struct solver
{
private:
  // vector<vector<complex<double>>> matrix;
  // vector<double> RHS;  //电压电流没有虚数，代考察是否正确
  sp_cx_mat matrix;
  cx_vec RHS;

  sp_cx_mat plot_matrix; // n-1
  cx_vec LHS_data;       // n-1
  cx_vec plot_RHS;       // n-1
  // vec<QString> L;//好像没有string类
  vector<QString> LHS; //节点命名
  // plot data:
  int gnd_idx;
  double init_hz;

public:
  QString matrixed_res;
  QVector<double> plot_xdata;
  QVector<double> plot_ydata;
  QString plot_xlabel;
  QString plot_ylabel;
  int plot_type; // is ac tran
  solver();
  ~solver();

  void bulid_matrix(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &sum_sz, int &branch_sz, vector<int> &plot, vector<double> &tran); //填充 matrix lhs rhs

  QString get_matrix_info(vector<double> &sim, int branch_sz, unordered_map<string, int> &node2row, vector<PulseDevice> &pulse_dev_list, vector<int> &plot, vector<double> &tran); //将 matrix lhs rhs输出为一个整体
  QString get_matrix_res(unordered_map<string, int> &node2row, bool is_tran);                                                                                                      //输出结果 hw5 计算矩阵结果：vector<complex<double>>,并将其转换为QString输出
  QString build_and_get_matrix_info(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &sum_sz, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran);
  void clear_matrix_imag();
  void show_node2row(unordered_map<string, int> &node2row);
  vector<pair<int, int>> get_matrix_imag_idx();
  void matrix2qstring(QString &res, unordered_map<string, int> &node2row, bool is_tran);
  bool check_have_init_hz(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list);
};
#endif // SOLVER_H