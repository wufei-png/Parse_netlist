#ifndef CIRCUIT_H
#define CIRCUIT_H
#include <QString>

#include "parse/parse.h"
#include "solver/solver.h"
#include "devices/two_port_device.h"
#include "devices/four_port_device.h"
#include "devices/pulse_device.h"
#include "plotter/plotter.h"
#include <unordered_map>
//考虑到rcl类都比较类似，直接在类成员里使用枚举类型区分
// E7 4 0 1 2 13如何判断是电压控制的电流源
using namespace std;
struct circuit
{
private:
  bool has_parsed;
  bool has_matrixed; //这两个bool既可以在circuit中，也可以在parse和solver中，优劣势?

  // bool simulation;   //是否有类似.dc的模拟

  // bool is_dc;
  /*
  分成初始状态和模拟动态状态 (针对dc，ac情况并没有初始频率，也就不打印初始matrix)
  一个电路要么是交流，要么是直流??
   dc初始矩阵:需要把虚部清除，遍历即可，不需要记录信息。
   dc模拟时:要先对dc模拟的name在LHS中匹配找到对应的行(parser中实现)，然后修改RHS中的值即可;电压电流源均适用
   ac初始矩阵:需要遍历，记录哪些位置具有虚部信息（vector<pair<int, int>>),貌似并没有初始情况，那么就直接模拟
   ac模拟时，调用写好的函数 for循环 乘以不同的频率即可。相比dc之所以不需要匹配LHS,是因为电路中只有一个频率，所有虚部都受到影响????
  个人认为这个二维vector不需要在circuit中记录，有点浪费空间，规模较小，遍历时间比较短，每次matrix时候重新算即可。

  以什么样的形式遍历展现到ui中,同
  时每个值都要计算对应时刻的结果。

  电路固有的，偏器件的用类表示，模拟，tran命令的信息用一个自有类型 int/double 的vector表示
  */
  file_parse *m_parser;
  solver *m_solver;
  plotter *m_plotter;
  vector<TwoPortDevice> two_dev_list; // c和l仅仅是值，build_matrix的时候输入为wj,然后对于c和l乘以它
  vector<FourPortDevice> four_dev_list;
  vector<PulseDevice> pulse_dev_list;
  //对于node,使用unordermap node2row string int
  unordered_map<string, int> node2row;
  vector<int> plot;
  vector<double> tran;
  vector<double> sim; //记录模拟的相关信息
  int sum_sz;
  int branch_sz; // r c l v i中，l和v需要开branch行 而为什么不顺序遇到 l v直接加到矩阵里，是为了和课件的矩阵形式一致？branch放在最后
public:
  circuit(QFile *file1);
  ~circuit();
  QString get_parse_res();
  QString get_matrix_res();
  void plot_res();
};
#endif // CIRCUIT__H