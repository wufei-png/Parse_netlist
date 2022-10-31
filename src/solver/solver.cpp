#include "solver.h"
using namespace std;
/*
先展现电路初始矩阵

随后dc模拟

*/
solver::solver()
{
  // matrix.clear();
  LHS.clear();
  RHS.reset();
  matrixed_res.clear();
  init_hz = 0;
  matrix.reset();
  plot_xdata.clear();
  plot_ydata.clear();
  plot_type = false;
  gnd_idx = -1;
}
solver::~solver()
{
}
void solver::bulid_matrix(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &sum_sz, int &branch_sz, vector<int> &plot, vector<double> &tran) //填充 matrix lhs rhs
{
  vector<int> cl_idx;
  // matrix.resize(sum_sz, vector<complex<double>>(sum_sz, 0));
  matrix.set_size(sum_sz, sum_sz);
  LHS.resize(sum_sz, "");
  // RHS.resize(sum_sz, 0);

  // vec R_tmp(sum_sz); //改进 ?? set_size
  RHS.set_size(sum_sz);
  RHS.zeros(); //还必须得清零，和matrix不一样，不然是随机数，我晕了
  int branch_i = sum_sz - branch_sz;
  //首先处理最容易的LHS中的ma部分
  for (int i = 0; i < branch_i; i++)
  {
    LHS[i] = "V" + QString(to_string(i).c_str());
  }
  //其次处理matrix RHS和LHS中的branch部分
  int cl_i = 0;
  for (auto &elem : two_dev_list)
  {
    switch (elem.type) //// r c l v i 0-4 node标号必须得从0开始 [nd1][nd2]=value是对的，具体原因是网表的定义（映射到实际器件中的方向）
    {
    case 0:
      cout << "You entered R. \n";
      matrix(elem.nd1, elem.nd1) += complex<double>{1 / elem.value, 0};
      matrix(elem.nd2, elem.nd2) += complex<double>{1 / elem.value, 0};
      matrix(elem.nd1, elem.nd2) -= complex<double>{1 / elem.value, 0};
      matrix(elem.nd2, elem.nd1) -= complex<double>{1 / elem.value, 0};

      // matrix[elem.nd1][elem.nd1] += complex<double>{1 / elem.value, 0};
      // matrix[elem.nd2][elem.nd2] += complex<double>{1 / elem.value, 0};
      // matrix[elem.nd1][elem.nd2] -= complex<double>{1 / elem.value, 0};
      // matrix[elem.nd2][elem.nd1] -= complex<double>{1 / elem.value, 0};
      break;
    case 1:
      cout << "You entered C. \n";
      cl_idx.push_back(cl_i);
      matrix(elem.nd1, elem.nd1) += complex<double>{0, 2 * M_PI * elem.value};
      matrix(elem.nd2, elem.nd2) += complex<double>{0, 2 * M_PI * elem.value};
      matrix(elem.nd1, elem.nd2) -= complex<double>{0, 2 * M_PI * elem.value};
      matrix(elem.nd2, elem.nd1) -= complex<double>{0, 2 * M_PI * elem.value};

      // matrix[elem.nd1][elem.nd1] += complex<double>{0, 2 * M_PI * elem.value};
      // matrix[elem.nd2][elem.nd2] += complex<double>{0, 2 * M_PI * elem.value};
      // matrix[elem.nd1][elem.nd2] -= complex<double>{0, 2 * M_PI * elem.value};
      // matrix[elem.nd2][elem.nd1] -= complex<double>{0, 2 * M_PI * elem.value};
      break;
    case 2:
      cout << "You entered L.\n";
      cl_idx.push_back(cl_i);
      matrix(branch_i, elem.nd1) = complex<double>{1, 0};
      matrix(branch_i, elem.nd2) = complex<double>{-1, 0};
      matrix(branch_i, branch_i) = complex<double>{0, -1 * 2 * M_PI * elem.value};
      matrix(elem.nd1, branch_i) = complex<double>{1, 0};
      matrix(elem.nd2, branch_i) = complex<double>{-1, 0};

      // matrix[branch_i][elem.nd1] = complex<double>{1, 0};
      // matrix[branch_i][elem.nd2] = complex<double>{-1, 0};
      // matrix[branch_i][branch_i] = complex<double>{0, -1 * 2 * M_PI * elem.value};
      // // qDebug() << matrix[branch_i][branch_i].imag();
      // matrix[elem.nd1][branch_i] = complex<double>{1, 0};
      // matrix[elem.nd2][branch_i] = complex<double>{-1, 0};

      LHS[branch_i] = "I(" + elem.name + ")";
      elem.branch_idx = branch_i;
      branch_i++;
      break;
    case 3:
      cout << "You entered V.\n";
      matrix(branch_i, elem.nd1) = complex<double>{1, 0};
      matrix(branch_i, elem.nd2) = complex<double>{-1, 0};
      matrix(elem.nd1, branch_i) = complex<double>{1, 0};
      matrix(elem.nd2, branch_i) = complex<double>{-1, 0};

      // matrix[branch_i][elem.nd1] = complex<double>{1, 0};
      // matrix[branch_i][elem.nd2] = complex<double>{-1, 0};
      // matrix[elem.nd1][branch_i] = complex<double>{1, 0};
      // matrix[elem.nd2][branch_i] = complex<double>{-1, 0};

      LHS[branch_i] = "I(" + elem.name + ")";

      // RHS[branch_i] = elem.value;
      RHS(branch_i) = elem.value;
      elem.branch_idx = branch_i;
      branch_i++;
      break;
    case 4:
      cout << "You entered I.\n";

      RHS(elem.nd1) = -1 * elem.value;
      RHS(elem.nd2) = elem.value;

      // RHS[elem.nd1] = -1 * elem.value;
      // RHS[elem.nd2] = elem.value;
      break;
    default:
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("two_device type error,code bug!"));
    }
    cl_i++;
  }
  for (auto &elem : four_dev_list)
  {                    // s=jw=2Πf
    switch (elem.type) //// r c l v i 0-4 node标号必须得从0开始 [nd1][nd2]=value是对的，具体原因是网表的定义（映射到实际器件中的方向）
    {
    case 0:
      cout << "You entered G. \n";

      matrix(elem.nd1, elem.ncd1) += complex<double>{elem.value, 0};
      matrix(elem.nd1, elem.ncd2) -= complex<double>{elem.value, 0};
      matrix(elem.nd2, elem.ncd1) -= complex<double>{elem.value, 0};
      matrix(elem.nd2, elem.ncd2) += complex<double>{elem.value, 0};

      // matrix[elem.nd1][elem.ncd1] += complex<double>{elem.value, 0};
      // matrix[elem.nd1][elem.ncd2] -= complex<double>{elem.value, 0};
      // matrix[elem.nd2][elem.ncd1] -= complex<double>{elem.value, 0};
      // matrix[elem.nd2][elem.ncd2] += complex<double>{elem.value, 0};
      break;
    case 1:
      cout << "You entered E. \n";

      matrix(branch_i, elem.nd1) = complex<double>{1, 0};
      matrix(branch_i, elem.nd2) = complex<double>{-1, 0};
      matrix(branch_i, elem.ncd1) = complex<double>{-elem.value, 0};
      matrix(branch_i, elem.ncd2) = complex<double>{elem.value, 0};
      matrix(elem.nd1, branch_i) = complex<double>{1, 0};
      matrix(elem.nd2, branch_i) = complex<double>{-1, 0};

      // matrix[branch_i][elem.nd1] = complex<double>{1, 0};
      // matrix[branch_i][elem.nd2] = complex<double>{-1, 0};
      // matrix[branch_i][elem.ncd1] = complex<double>{-elem.value, 0};
      // matrix[branch_i][elem.ncd2] = complex<double>{elem.value, 0};
      // matrix[elem.nd1][branch_i] = complex<double>{1, 0};
      // matrix[elem.nd2][branch_i] = complex<double>{-1, 0};
      elem.branch_idx = branch_i;
      LHS[branch_i] = "I(" + elem.name + ")";
      branch_i++;
      break;

    default:
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("four_device type error,code bug!"));
    }
  }
  for (auto &elem : pulse_dev_list)
  {                    // s=jw=2Πf
    switch (elem.type) //// r c l v i 0-4 node标号必须得从0开始 [nd1][nd2]=value是对的，具体原因是网表的定义（映射到实际器件中的方向）
    {
    case 0:
      cout << "You entered pulse V. \n";

      matrix(branch_i, elem.nd1) = complex<double>{1, 0};
      matrix(branch_i, elem.nd2) = complex<double>{-1, 0};
      matrix(elem.nd1, branch_i) = complex<double>{1, 0};
      matrix(elem.nd2, branch_i) = complex<double>{-1, 0};

      // matrix[branch_i][elem.nd1] = complex<double>{1, 0};
      // matrix[branch_i][elem.nd2] = complex<double>{-1, 0};
      // matrix[elem.nd1][branch_i] = complex<double>{1, 0};
      // matrix[elem.nd2][branch_i] = complex<double>{-1, 0};

      LHS[branch_i] = "I(" + elem.name + ")";

      // RHS[branch_i] = elem.value;
      RHS(branch_i) = elem.start_value;
      elem.branch_idx = branch_i;
      branch_i++;
      break;

    case 1:
      cout << "You entered pulse I. \n";
      RHS(elem.nd1) = -1 * elem.start_value;
      RHS(elem.nd2) = elem.start_value;
      break;

    default:
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("pulse dev type error,code bug!"));
    }
  }

  if (check_have_init_hz(two_dev_list, four_dev_list))
  {
    vector<pair<int, int>> imag_idxs = get_matrix_imag_idx();
    for (auto &elem : imag_idxs)
    {
      // matrix[elem.first][elem.second].imag(matrix[elem.first][elem.second].imag() * init_hz);
      //.set_imag( X ) .insert_rows( row_number, X )
      matrix(elem.first, elem.second) *= {1, init_hz};
    }
  }
  if (tran[0] == 1) //有tran命令 构建plot_matrix
  {
    plot_matrix = matrix;
    plot_RHS = RHS;
    for (auto &idx : cl_idx)
    {
      TwoPortDevice elem = two_dev_list[idx];
      if (elem.type == 1) // c
      {
        plot_matrix(elem.nd1, elem.nd1) -= complex<double>{0, 2 * M_PI * elem.value};
        plot_matrix(elem.nd2, elem.nd2) -= complex<double>{0, 2 * M_PI * elem.value};
        plot_matrix(elem.nd1, elem.nd2) += complex<double>{0, 2 * M_PI * elem.value};
        plot_matrix(elem.nd2, elem.nd1) += complex<double>{0, 2 * M_PI * elem.value};
        qDebug() << print_item(plot_matrix, "debug");

        plot_matrix(elem.nd1, elem.nd1) += complex<double>{elem.value / tran[1], 0};
        plot_matrix(elem.nd2, elem.nd2) += complex<double>{elem.value / tran[1], 0};
        plot_matrix(elem.nd1, elem.nd2) -= complex<double>{elem.value / tran[1], 0};
        plot_matrix(elem.nd2, elem.nd1) -= complex<double>{elem.value / tran[1], 0};

        plot_RHS(elem.nd1) += complex<double>{elem.value / tran[1], 0};
        plot_RHS(elem.nd2) -= complex<double>{elem.value / tran[1], 0};
      }
      else // l
      {
        plot_matrix(elem.branch_idx, elem.branch_idx) = complex<double>{-1 * elem.value / tran[1], 0};
        plot_RHS(elem.branch_idx) = complex<double>{-1 * elem.value / tran[1], 0};
      }
    }
    qDebug() << print_item(plot_matrix, "debug");
  }
}
bool solver::check_have_init_hz(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list)
{
  bool res = false;
  for (auto &x : two_dev_list) //因为parse中只处理了双端器件的初始hz，就不写检测四端得了
  {
    if (x.init_hz != 0)
    {
      if (init_hz == 0)
      {
        res = true;
        init_hz = x.init_hz;
      }
      else if (init_hz != init_hz)
      {
        res = false;
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("Conflicting initial frequencies!"));
      }
    }
  }
  return res;
}
void solver::matrix2qstring(QString &res, unordered_map<string, int> &node2row, bool is_tran)
{
  // qDebug() << "matrix2qstring";
  if (is_tran)
  {
    res += "tran matrix info:\n ";
    int sz = plot_matrix.n_rows;
    res += "LHS:\n [ ";
    for (int i = 0; i < sz; i++)
    {
      res += LHS[i] + " ";
    }
    res += "]\n";
    // RHS.print("debug");
    res += print_item(plot_matrix, "tran MNA Matrix:");
    res += print_item(plot_RHS, "tran RHS Matrix:");
    // res += print_item(LHS, "LHS Matrix:");

    res += get_matrix_res(node2row, is_tran);
    return;
  }

  else
  {
    res += "matrix info:\n ";
    int sz = matrix.n_rows;
    res += "LHS:\n [ ";
    for (int i = 0; i < sz; i++)
    {
      if (i == gnd_idx)
        continue;
      res += LHS[i] + " ";
    }
    res += "]\n";
    // RHS.print("debug");
    res += print_item(matrix, "MNA Matrix:");
    res += print_item(RHS, "RHS Matrix:");
    // res += print_item(LHS, "LHS Matrix:");

    res += get_matrix_res(node2row, is_tran);
    return;
  }
  return;
}

QString solver::get_matrix_info(vector<double> &sim, int start_branch_sz, unordered_map<string, int> &node2row, vector<PulseDevice> &pulse_dev_list, vector<int> &plot, vector<double> &tran) //将 matrix lhs rhs输出为一个整体 [matrix] | LHS | RHS
//找到一个最大长度对齐|但是好像有bug，可能是ui问题，对不齐
{ // dc ac 初始
  QString res;
  if (plot[0] == 0) // dc tran不需要，没有频率一说
  {
    clear_matrix_imag();
  }
  else
  {
    res += "init state of ac circuit's HZ==1 \n ";
  }
  // qDebug() << "clear_matrix_imag";
  matrix2qstring(res, node2row, 0);
  int showed_idx = plot[3];
  int read_data_idx = showed_idx;
  if (showed_idx > gnd_idx)
    read_data_idx--;
  if (sim[1] == 0 && tran[0] == 0) //如果没有模拟和tran命令，无论dc，ac返回初始状态即可
    return res;

  // qDebug() << "bug1";

  else if (sim[1] == 1) //根据dc还是ac模拟来 dc ac的数据改变规律不同，for循环的逻辑不同，需要分开写，但是dc的V I的数据都一样改变，因此公用,但是sim长度不同，需要从末尾取
  {
    if (sim[0] == 0 && plot[1] == 0) // dc
    {
      if (sim[2] == 0) // v
        plot_xlabel = "V(dc)";
      else
        plot_xlabel = "I(dc)";
      plot_ylabel = LHS[showed_idx] + "(dc)";
      plot_type = 0;
      int sim_sz = sim.size();
      double start = sim[sim_sz - 3];
      double end = sim[sim_sz - 2];
      double step = sim[sim_sz - 1];
      if (sim[2] != -1) //得是V或者I
        for (double value = start; value <= end; value += step)
        {
          qDebug() << "startqwwwwwwww" << start << step << end;
          if (sim[2] == 0) // v
          {
            RHS(sim[3] + start_branch_sz) = value;
          }
          else // I
          {
            RHS(sim[3]) = value;
            RHS(sim[4]) = -value;
          }
          matrix2qstring(res, node2row, 0); // update matrix and calculate
          plot_xdata.push_back(value);
          plot_ydata.push_back(complex_norm(LHS_data[read_data_idx], 0));
        }
    }
    else if (sim[0] == 1 && plot[1] == 1) // ac V I一视同仁
    {
      // qDebug() << "bug2";
      if (sim[2] == 0) // v
        plot_xlabel = "V(ac)";
      else
        plot_xlabel = "I(ac)";
      plot_ylabel = LHS[showed_idx] + "(ac)";
      plot_type = 1;

      vector<pair<int, int>> imag_idxs = get_matrix_imag_idx();
      // vector<vector<complex<double>>> raw_matrix = matrix;
      sp_cx_mat raw_A = matrix;
      double start = sim[4];
      double end = sim[5];
      double step;
      while (start < end)
      {
        // qDebug() << "bug3";
        step = (sim[3] * start - start) / (sim[6] - 1);
        for (double value = start; value < sim[3] * start; value += step) //左闭右开
        {
          for (auto &elem : imag_idxs)
          {
            // matrix[elem.first][elem.second].imag(raw_matrix[elem.first][elem.second].imag() * value);
            matrix(elem.first, elem.second) = {matrix(elem.first, elem.second).real(), value * raw_A(elem.first, elem.second).imag()};
          }
          matrix2qstring(res, node2row, 0);

          plot_xdata.push_back(value);
          plot_ydata.push_back(complex_norm(LHS_data[read_data_idx], 0));
        }
        start *= sim[3];
      }
    }
  }
  else if (tran[0] == 1 && plot[1] == 2) // tran
  {
    if (plot[2] == 0) // v
      plot_xlabel = "V(tran)";
    else
      plot_xlabel = "I(tran)";
    plot_ylabel = LHS[showed_idx] + "(tran)";
    plot_type = 2;
    
    plot_matrix.shed_col(gnd_idx);
    plot_matrix.shed_row(gnd_idx);
    plot_RHS.shed_row(gnd_idx);
    for (auto &elem : pulse_dev_list)
    {
      elem.branch_idx--;
    }
    if (pulse_dev_list.empty())
    {
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("pulse_dev_list empty!"));
      res += "pulse_dev_list empty!";
      return res;
    }

    cx_vec raw_RHS = plot_RHS;

    // raw_RHS保存后，首先跑dc仿真
    matrix2qstring(res, node2row, 0);
    plot_RHS %= LHS_data;

    double step = tran[1];
    double end = tran[2];
    //构建pulse的函数

    //构建pulse的函数
    for (double time = 0; time < end; time += step)
    {
      for (auto &elem : pulse_dev_list)
      {
        plot_RHS[elem.branch_idx] = elem.get_value(time + step); // rhs中是time加step，其他都是time
      }
      res += doubleToQString(time);
      matrix2qstring(res, node2row, 1);
      plot_xdata.push_back(time);
      plot_ydata.push_back(complex_norm(LHS_data[read_data_idx], 1));

      plot_RHS = raw_RHS % LHS_data;

      //% element - wise multiplication of two objects(Schur product)
    }
  }
  return res;
}

QString solver::get_matrix_res(unordered_map<string, int> &node2row, bool is_tran) //输出结果 hw5 计算矩阵结果：vector<complex<double>>,并将其转换为QString输出
{
  //找gnd，如果找不到，那么我们默认数字0是gnd。
  // .insert_cols(col_number, X)(member functions of Mat, Row and Cube)
  //
  // .shed_row(row_number)
  // .shed_col(row_number)
  // to do solve the matrix

  QString res("\n matrix result:\n ");
  // qDebug() << "gnd:" << gnd_idx;

  // sp_cx_mat col_tmp(sz, 1);
  // sp_cx_mat row_tmp(1, sz);
  // for (int i = 0; i < sz; i++)
  // {
  //   row_tmp(0, i) = matrix(gnd_idx, i);
  //   col_tmp(i, 0) = matrix(i, gnd_idx);
  //   qDebug() << i << ":" << complex2qstring(row_tmp(0, i)) << complex2qstring(col_tmp(i, 0));
  // }
  sp_cx_mat matrix_cal;
  cx_vec RHS_cal;
  if (!is_tran)
  { // tran lhs rhs前后依赖的原因造成了这一个差异
    matrix_cal = matrix;
    RHS_cal = RHS;

    matrix_cal.shed_col(gnd_idx);
    matrix_cal.shed_row(gnd_idx);
    RHS_cal.shed_row(gnd_idx);
  }
  else
  {
    matrix_cal = plot_matrix;
    RHS_cal = plot_RHS;
  }

  // matrix.print("have gnd");
  bool status = spsolve(LHS_data, matrix_cal, RHS_cal); // use default solver
  if (status == false)
  {
    cout << "no solution" << endl;
    QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("can not find circuit solution ,return!"));
    return "can not find circuit solution ,return!";
  }
  else
  {
    res += print_item(LHS_data, "solution");
  }

  // matrix.insert_cols(1, col_tmp);
  // matrix.insert_rows(1, row_tmp);
  // print_item(matrix, "has gnd");

  return res;
}
void solver::show_node2row(unordered_map<string, int> &node2row)
{
  matrixed_res += "The node mapping relationship is: \n";
  for (auto x : node2row)
  {
    matrixed_res += QString("node name:") + x.first.c_str() + QString(" Corresponding number (in the row):") + int2qstring(x.second) + QString("\n");
    // matrixed_res += QString(x.first.c_str());
  }
  matrixed_res += "we need to find where gnd is\n";
  // find gnd idx

  bool has_zero = false;
  for (auto &elem : node2row)
  {
    if (elem.first == "0")
      has_zero = true;
    if (check_match_case_insensitive(elem.first, "gnd"))
    {
      gnd_idx = elem.second;
      break;
    }
  }
  if (gnd_idx == -1)
  {
    if (!has_zero)
    {
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("can not find gnd ,return!"));
    }
    else
      gnd_idx = node2row["0"];
  }
  // find gnd idx
}
QString solver::build_and_get_matrix_info(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &sum_sz, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran)
{
  bulid_matrix(two_dev_list, four_dev_list, pulse_dev_list, sum_sz, branch_sz, plot, tran);
  // int sz=dev_list.size();
  if (init_hz != 0)
    matrixed_res += "The initial frequency of the circuit is: " + doubleToQString(init_hz) + "\n";
  show_node2row(node2row);
  // qDebug() << "show_node2row";
  matrixed_res += get_matrix_info(sim, sum_sz - branch_sz, node2row, pulse_dev_list, plot, tran);
  return matrixed_res;
}
void solver::clear_matrix_imag()
{
  int sz = matrix.n_rows;
  // for (int i = 0; i < sz; i++)
  // {
  //   for (int j = 0; j < sz; j++)
  //   {
  //     matrix[i][j].imag(0);
  //   }
  // }
  // qDebug() << "clear_matrix_imaging" << sz;
  // int sz = matrix.size();
  for (int i = 0; i < sz; i++)
  {
    for (int j = 0; j < sz; j++)
    {
      matrix(i, j) *= {1, 0};
    }
  }
}
vector<pair<int, int>> solver::get_matrix_imag_idx()
{
  vector<pair<int, int>> res;
  // int sz = matrix.size();
  int sz = matrix.n_rows;
  for (int i = 0; i < sz; i++)
  {
    for (int j = 0; j < sz; j++)
    {
      // if (matrix[i][j].imag() != 0)
      if (matrix(i, j).imag() != 0)
        res.push_back({i, j});
    }
  }
  return res;
}