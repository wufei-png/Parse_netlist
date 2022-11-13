// Mna右侧是电压源，电流源
// 规定流出节点的电流为正
// 规定求解的电流方向都是低序号节点指向高序号节点
//以上信息首先输出
/***
 * hw4
 * 程序逻辑：
      输入为文件描述符来构建类，由于testbvench简单，以及parse类里并没有保存解析后的信息，我这里直接在circuit类函数里解析所有行了。

        构造函数：

        主函数：
        解析并构建矩阵

        输出函数：
      将二维指针转换为QString返回给window

首先输出提示信息


  circuit管理类，尽量减少底层成员变量 包括parse类 solver类：建立矩阵 计算矩阵 hw4属于建立矩阵范畴
 */

/*problem:
const QString L 变量重复
*/
#include "circuit.h"
using namespace std;

circuit::circuit(QFile *file)
{
  m_parser = new file_parse(file); //在mainwindow已经判断了file不为空才会构造circuit
  m_solver = new solver();
  sum_sz = 0;
  branch_sz = 0;
  has_parsed = false;
  has_matrixed = false;
  sim.resize(3);
  sim[0] = 0;  // dc
  sim[1] = 0;  //没有模拟
  sim[2] = -1; //电压模拟
  plot.resize(4, 0);
  tran.resize(3, 0);
  two_dev_list.clear(); //保险起见可以的
  four_dev_list.clear();
  node2row.clear();
}
circuit::~circuit()
{
  delete m_parser;
}
QString circuit::get_parse_res() //不支持修改的话，就做一个缓存，只parse一次
{
  if (has_parsed)
    return m_parser->parsed_res;
  else
  {
    has_parsed = true;
    return m_parser->parse(two_dev_list, four_dev_list, pulse_dev_list, sum_sz, branch_sz, sim, node2row, plot, tran, this);
  }
}

QString circuit::get_matrix_res()
{
  // qDebug() << 111;
  if (has_parsed)
  {
    if (has_matrixed)
      return m_solver->matrixed_res;
    else
    {
      has_matrixed = true;
      return m_solver->build_and_get_matrix_info(two_dev_list, four_dev_list, pulse_dev_list, sum_sz, branch_sz, sim, node2row, plot, tran);
    }
  }

  else
  {
    qDebug() << 123;
    QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("can matrix circuit before parsed netlist"));
  }

  return QString("can matrix circuit before parsed netlist");
}
void circuit::plot_res()
{
  if (has_matrixed)
  {
    return m_plotter->plot_it(m_solver->plot_xdata, m_solver->plot_ydata, m_solver->plot_xlabel, m_solver->plot_ylabel, m_solver->plot_type);
  }

  else
  {
    QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("has not matrix ,can not plot it!"));
    return;
  }
  // else
  //   m_solver->plot
}