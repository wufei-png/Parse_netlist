/*
  file_parse
  */
/*
r c l class
circuit class
solver class 管理矩阵 求解
VIN gnd node_1 DC 5
.print V(node_1) 网表的格式严格规定
qtext怎么分成两个窗口
功能：只能解析一行的数据完整的情形
继承关系 代码结构?

语法规范（检测）：
.end结束后不能有新的东西
  最后一行为end
  器件名称不能重复
  支持解析2k
  不支持2w,但是原理一致，懒得写了
  第一个非注释行 为标题，非空
  空行跳过
  节点名称可以重复，器件名称不可以
  一些报错检测可见testbench实例

解析逻辑：
查看第一个字符来区分*和.还是器件，具体针对.和器件情形继续看格式等是否正确
默认.dc命令会在器件命令之后，不然找不到对应的器件 find_idx_from_devlist

功能问题
会有.DC I 0.5 2 0.1也能解析对的尴尬局面
节点名称只是数字
模拟只能模拟
遇到的问题和知识点：
没有换行符不能打印，Qdebug不需要换行，因为qdebug自带换行。
只在网表里驱动仿真，而不是qt ui中
正则大小写[oO]   ||   setCaseSensitivity（CaseInsensitive//0）
if嵌套太多

增加删减文件要修改pro
如果起了Qapplication，那么cout必须要endl才能输出
OCT 倍频程变量（变量按倍频程规律变化进行扫描）
不打开文件点击parse报错
不点击parse就matrix报错
错误处理仅仅是弹窗
使用稀疏矩阵存储
器件的dc ac属性与模拟不符，那么报错并设置bool?
Analysis Command: AC (name: DEC ;Start:
*/

#include "parse.h"
#include "circuit/circuit.h"
using namespace std;

QString file_parse::get_cur_line()
{
  return QString(" line ") + int2qstring(parsed_nums) + QString(": ");
}
QString file_parse::dev_info(QStringList list)
{
  return QString("(Name: ") + list[0] + QString("; value: ") + list[3] + QString("; Node1: ") + list[1] + QString("; Node2: ") + list[2] + QString(" )");
}
// QString parse_dev(QStringList &strlist, bool isdot)
// {
//   if (!check_name(strlist[0]))
//     res += QString("原件命名有问题");

//   else //需要判断器件类型
//   {
//     QString name;
//     if (strlist[0][0] == QChar('V') || strlist[0][0] == QChar('v'))
//       name = Vol;
//     else if (strlist[0][0] == QChar('R') || strlist[0][0] == QChar('r'))
//       name = reg;
//     else
//       name = cap;
//     res += name + dev_info(strlist);
//   }
// }
void file_parse::check_print(const QString &str1, const QString &str2, QString &res)
{
  cout << str2.toStdString();
  int idx = 0;
  for (; idx < str2.size(); idx++)
  {
    if (str2[idx] == QChar('('))
      break;
  }

  QString q_str2_0 = str2.mid(0, idx);
  QString q_str2_1 = str2.mid(q_str2_0.size() + 1, str2.size() - q_str2_0.size() - 2);
  if ((!str1.contains(QRegExp("^[aADd][Cc]$"))) && (str1 != QString("unknow")))
    res += get_cur_line() + QStringLiteral("cannot find print info device type");
  else if ((!q_str2_0.contains(QRegExp("^[IiVv]\\(\\d+\\)$"))) && (!q_str2_1.contains(QRegExp("^[a-zA-Z]+_?\\d*$", Qt::CaseInsensitive))))
    res += get_cur_line() + QStringLiteral("the print info's value is illegal");
  else //正常
  {
    res += out_info + str1 + QString("; Type:") + q_str2_0 + QChar(' ') + q_str2_1 + QChar(')');
  }
}
bool file_parse::check_dev_name(QString &line, QString &res, bool simulation)
{
  // if (simulation)
  // {
  //   bool check = line.contains(QRegExp("^[Dd][Ee][Cc]$"));
  //   if (!check)
  //     res += get_cur_line() + QStringLiteral("after .ac is not dec ");
  //   return check;
  // }
  if (simulation && (line.contains(QRegExp("^[Dd][Ee][Cc]$")) || line.contains(QRegExp("^[Oo][Cc][Tt]$")) || line.contains(QRegExp("^[Ll][Ii][Nn]$")))) //
    return true;
  const QRegExp dev_rep("^[CcRrVvLlIi][a-zA-Z]*\\d*$"); // r c l
  if (line.contains(dev_rep))
  {
    string tmp = line.toStdString();
    if (dev_map.count(tmp) && (!simulation))
    {
      res += get_cur_line() + QStringLiteral("device name has existed");
      return false;
    }
    else
      return true;
  }
  res += get_cur_line() + QStringLiteral("divice name format not right: ");
  return false;
}
bool file_parse::check_dev_node_value(QStringList list, QString &res, bool simulation, unordered_map<string, int> &node2row) // sim参数代表不是dc模拟的 前两个是节点号，不应该含k或者小数点
{
  bool check;
  bool retu = true;
  int sz = list.size(); //改成sz以适配四端器件
  for (int i = 1; i < sz; i++)
  {
    if (!simulation && i != sz - 1) // node name  可以改进：如果是模拟情况，这个节点必须已经在哈希表中了
    {
      check = list[i].contains(QRegExp("^[+-]?\\d+$"));
      check = check || list[i].contains(QRegExp("^[a-zA-Z]+_?\\d*$", Qt::CaseInsensitive)); //如何实现_可有可无，但是如果有_，后面必须得有数字 node不一定是数字
      if (check)
      { //这个节点之前没有碰到过
        string tmp = list[i].toStdString();
        if (!node_map.count(tmp))
        { // to do 维持映射：比如说是node1,然后它在matrix里是代表第i行，那么就是node1:i 先来的先占 node2row string int
          node2row[tmp] = Node_nums;
          qDebug() << "node2row:" << list[i] << '\t' << Node_nums;
          node_map[tmp] = 1;
          Node_nums++;
        }
      }
      else
      {
        retu = false;
        res += get_cur_line() + " Node name is illegle ";
      }
    }
    else // digit
    {
      check = (list[i].contains(QRegExp("^[+-]?\\d+(\\.\\d+)?[kKpPmMuUFf]?$")) || list[i].contains(QRegExp("^[+-]?\\d+(\\.\\d+)?[Mm][Ee][Gg]$"))); //都是数字,带正负，可以带k 可以是小数 后续有需要u,m什么的直接添加即可 MEg
      check = check || list[i].contains(QRegExp("^[+-]?\\d+e[+-]?\\d+$"));                                                                         //带e的科学计数
    }

    if (!check)
    {
      res += get_cur_line() + "value is illeagle ";
      retu = false;
    }
  }
  return retu;
}
bool file_parse::check_dev_form(QStringList &strlist, QString &res, bool sim, vector<double> &sim_vec, bool &is_two, unordered_map<string, int> &node2row, double &init_hz)
{ //代码结构不合理，冗余
  bool right = true;
  if (!sim && strlist.size() == 6 && strlist[4].contains(QRegExp("^[Aa][Cc]$")) && strlist[5].contains(QRegExp("^\\d+$"))) //在.dc ac中会有其他逻辑检查语法，此处检查器件中的ac :v2 0 1 ac 10 1 //初始赫兹
  {
    qDebug() << "对了";
    init_hz = qstring2double(strlist[5]);
    sim_vec[0] = 1;
    strlist.removeAt(4);
    strlist.removeAt(4);
    if (strlist[0].contains(QRegExp("^[Vv][a-zA-Z]*\\d*$")))
      sim_vec[2] = 0;
    else if (strlist[0].contains(QRegExp("^[Ii][a-zA-Z]*\\d*$")))
      sim_vec[2] = 1;
    else
    { // unknow
      sim_vec[2] = -1;
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("ac device type is not V or I"));
    }
  }
  if (strlist.size() == 3)
    return false;
  bool is_ac = strlist[3].contains(QRegExp("^[Aa][Cc]$"));
  bool is_dc = strlist[3].contains(QRegExp("^[Dd][Cc]$"));
  if (!sim && strlist.size() == 5 && (is_ac || is_dc)) //在.dc ac中会有其他逻辑检查语法，此处检查器件中的ac :v2 0 1 ac 10
  {
    if (is_ac) //其实ac是什么器件没什么用处，因为改变的是matrix全局
      sim_vec[0] = 1;
    strlist.removeAt(3);
    if (strlist[0].contains(QRegExp("^[Vv][a-zA-Z]*\\d*$")))
      sim_vec[2] = 0;
    else if (strlist[0].contains(QRegExp("^[Ii][a-zA-Z]*\\d*$")))
      sim_vec[2] = 1;
    else
    { // unknow
      sim_vec[2] = -1;
      QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("ac device type is not V or I"));
    }
  }
  if (strlist.size() != 4)
  {
    if (!sim && strlist.size() == 6 && strlist[0].contains(QRegExp("^[GgEe][a-zA-Z]*\\d*$")) && check_dev_node_value(strlist, res, sim, node2row))
    {
      is_two = false;
      return true;
    }
    else
    {
      res += get_cur_line() + QStringLiteral("device parameter length is not right");
      return false;
    }
  }
  // cout << "器件名称没问题123" << endl;
  if (!check_dev_name(strlist[0], res, sim))
  {
    // res += QString("原件命名有问题");
    // qDebug() << "bugbug1";
    right = false;
  }
  // qDebug() << "right:" << right;
  // cout << "器件名称没问题" << endl;
  if (!check_dev_node_value(strlist, res, sim, node2row))
  {
    // res += get_cur_line() + QStringLiteral("device value error");
    right = false;
  }
  // qDebug() << "right:" << right;
  return right;
}

file_parse::file_parse(QFile *file1)
{
  parsed_nums = 0;
  empty_nums = 0;
  Device_nums = 0;
  Node_nums = 0;
  V_nums = 0;
  R_nums = 0;
  L_nums = 0;
  C_nums = 0;
  I_nums = 0;
  parsed_res.clear();
  end = false;
  // QString filename=QString("TB1.sp");
  // QString filepath=QString("./testbench/")
  file = file1;
  file->seek(0); //
  cout << &file;
}
file_parse::~file_parse()
{
}
void file_parse::show_dev_nums(QString &res)
{
  res += "device num:" + QString(Device_nums + '0') + " \n";
  res += "node num:" + QString(Node_nums + '0') + " \n";
  res += "R num:" + QString(R_nums + '0') + " \n";
  res += "C num:" + QString(C_nums + '0') + " \n";
  res += "L num:" + QString(L_nums + '0') + " \n";
}
QString file_parse::get_next_line()
{
  if (!file->atEnd())
  {
    QString str = file->readLine();
    str = str.trimmed();
    // cout << tmp.size();
    // cout << 6 << endl;
    return str;
    // qDebug() << value << endl;
  }
  else
    return QString("endwefkhpasuuhpor");
}

QString file_parse::parse_one_line(QString line, vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran)
{
  parsed_nums++;
  QString res;
  if (line == NULL) //空行
  {
    empty_nums++;
    res += QString("Empty line");
    return res;
  }
  if (line == "endwefkhpasuuhpor") //.end
  {
    return NULL;
  }
  // cout << 1 << endl;
  QStringList strlist = line.split(' ');
  // cout << parsed_nums << endl;
  if (end)
  {
    res += get_cur_line() + QStringLiteral("reached end,next is illegal");
    end = false; //提示一次就够了
  }
  // cout << 2 << endl;
  else if (line[0] == QChar('*')) //首先判断注释
  {
    cout << "解析到注释" << endl;
    // parsed_nums--;                 //用正常逻辑，其他的加，要if嵌套或者很多情况都要写了
    res += anno + line.mid(1, -1); //去掉空格
  }
  else if ((parsed_nums - empty_nums) == 1) //判断标题
  {
    cout << "解析到标题" << endl;
    res += title + line;
  }

  else if (line[0] == QChar('.'))
  { //判断开头为.的
    cout << "解析到." << endl;
    if (line.contains(QRegExp("^.[Ee][Nn][Dd]$")))
    {
      end = true;
      cout << "解析到end" << endl;
      res += end_str;
      return res;
    }
    else if (line.mid(0, 6).contains(QRegExp("^.[Tt][Rr][Aa][Nn] $")))
    {
      cout << "解析到tran" << endl;
      QString left = line.mid(6, -1); // left为剩余的
      QStringList left_list = left.split(' ');
      for (int i = 0; i < 2; i++)
      {
        if (!left_list[i].contains(QRegExp("^\\d+(\\.\\d+)?$")))
        {
          tran[0] = 0;
          res += get_cur_line() + "tran not all digit !";
          QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("tran not all digit!")); //
          return res;
        }
      }
      tran[0] = 1;
      tran[1] = qstring2double(left_list[0]);
      tran[2] = qstring2double(left_list[1]);
      qDebug() << "tran12:" << tran[1] << tran[2];
      res += "tran: step: " + left_list[0] + " end :" + left_list[1];
      return res;
    }
    else if (line.mid(0, 6).contains(QRegExp("^.[Pp][Ll][Oo][Tt] $"))) //之后的没检查句法
    {
      cout << "解析到plot" << endl;
      QString left = line.mid(6, -1); // left为剩余的
      QStringList left_list = left.split(' ');
      QString cur_node;
      bool is_v;
      bool is_i;
      if (left_list.size() != 2)
      {
        res += get_cur_line() + "plot size is not 2!";
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("plot size is not 2")); //
        return res;
      }
      bool is_tran = left_list[0].contains(QRegExp("^tran$", Qt::CaseInsensitive));
      bool is_dc = left_list[0].contains(QRegExp("^dc$", Qt::CaseInsensitive));
      bool is_ac = left_list[0].contains(QRegExp("^ac$", Qt::CaseInsensitive));
      if (!(is_tran || is_dc || is_ac))
      {
        res += get_cur_line() + "plot is not ac or dc or tran !";
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("plot is not ac or dc or tran")); //
        return res;
      }

      if ((sim[0] == 0 && is_ac) || (sim[0] == 1 && is_dc))
      {
        res += get_cur_line() + "plot dc or ac is not match !";
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("plot dc or ac is not match ")); //
        return res;
      }

      if (is_dc)
        plot[1] = 0;
      else if (is_ac)
        plot[1] = 1;
      else
        plot[1] = 2;

      is_v = (left_list[1].contains(QRegExp("^v[a-z]*\\d*\\(\\d+\\)$", Qt::CaseInsensitive)) || left_list[1].contains(QRegExp("^v[a-z]*\\d*\\([a-z]+\\d*\\)$", Qt::CaseInsensitive)));
      is_i = (left_list[1].contains(QRegExp("^i[a-z]*\\d*\\(\\d+\\)$", Qt::CaseInsensitive)) || left_list[1].contains(QRegExp("^i[a-z]*\\d*\\([a-z]+\\d*\\)$", Qt::CaseInsensitive)));
      if (!(is_v || is_i))
      {
        res += get_cur_line() + "plot is not dc or ic!";
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("plot is not dc or ic ")); //
        return res;
      }

      if (is_i)
        plot[2] = 1;

      int start = left_list[1].indexOf('(');
      int end = left_list[1].indexOf(')');
      cur_node = left_list[1].mid(start + 1, end - start - 1);
      plot[3] = node2row[cur_node.toStdString()];

      plot[0] = 1;
      QString type_tmp = is_v ? "V" : "I";
      res += "plot type: " + left_list[0] + " dev type: " + type_tmp + " node: " + cur_node;
      qDebug() << "111111111111111111";
      for (auto &x : plot)
        qDebug() << x;
      qDebug() << "111111111111111111";
      return res;
    }
    else if (line.mid(0, 4).contains(QRegExp("^.[Oo][Pp] $"))) //之后的没检查句法
    {
      res += analy + line.mid(4, -1);
      return res;
    }
    // DC print testbench2

    else if (line.mid(0, 4).contains(QRegExp("^.[aADd][Cc] $"))) // dc ac
    {
      cout << "解析到dc" << endl;
      QString left = line.mid(4, -1); // left为剩余的
      QStringList left_list = left.split(' ');
      bool right = true;
      bool tmp = 0;    //为了通过编译，没用
      double tmp1 = 0; //为了通过编译，没用
      right = check_dev_form(left_list, res, 1, sim, tmp, node2row, tmp1);
      if (right)
      {
        qDebug() << "bugbug1";
        sim[1] = 1;
        bool is_dc = line.mid(0, 4).contains(QRegExp("^.[Dd][Cc] $"));
        if ((is_dc && sim[0] == 1) || ((!is_dc) && sim[0] == 0))
        {
          QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("simlulation dc or ac is not matched with device describ")); //
          sim[1] = 0;
        }
        if (is_dc) //判断dc后是不是V或者I
        {
          if (left_list[0].contains(QRegExp("^[Vv][a-zA-Z]*\\d*$")))
            sim[2] = 0;
          else if (left_list[0].contains(QRegExp("^[Ii][a-zA-Z]*\\d*$")))
            sim[2] = 1;
          else
          { // unknow
            sim[2] = -1;
            sim[1] = 0;
            QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr(".dc/ac is not v or i"));
          }
        }
        qDebug() << "bugbug2";
        int step = 0; //模拟步长
        if ((!is_dc))
        {
          if (!(left_list[0].contains(QRegExp("^[Dd][Ee][Cc]$")) || left_list[0].contains(QRegExp("^[Oo][Cc][Tt]$")) || left_list[0].contains(QRegExp("^[Ll][Ii][Nn]$")))) // devc oct lin
          {
            QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("after .ac is not dec "));
            qDebug() << ">?????";
            sim[1] = 0;
          }
          else if (left_list[0][0] == QChar('D') || left_list[0][0] == QChar('d'))
            step = 10;
          else if (left_list[0][0] == QChar('D') || left_list[0][0] == QChar('d'))
            step = 8;
          else
            step = 1;
        }
        if (sim[1] == 1) //通过了初步检查
        {
          if (is_dc)
          {
            qDebug() << "bugbug3";
            qDebug() << left_list[0];
            for (auto x : two_dev_list)
              qDebug() << x.name;
            int idx = find_idx_from_devlist(two_dev_list, left_list[0]); //从hw5的tb1来看好像网表不需要匹配，一种植物
            qDebug() << "bugbug4";
            if (idx == -1)
            {
              sim[1] = 0;
              QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("cannot find dc simulation dev name!"));
            }
            else //最终通过了语法检查
            {
              if (sim[2] == 1) // I
              {
                sim.push_back(two_dev_list[idx].nd1);
                sim.push_back(two_dev_list[idx].nd2);
              }
              else // V
              {
                qDebug() << "bugbug5";
                sim.push_back(two_dev_list[idx].branch_idx);
              }
              sim.push_back(qstring2double(left_list[1]));
              sim.push_back(qstring2double(left_list[2]));
              sim.push_back(qstring2double(left_list[3]));
            }
          }
          else // ac
          {
            sim.push_back(qstring2double(left_list[1]));
            sim.push_back(qstring2double(left_list[2]));
            sim.push_back(qstring2double(left_list[3]));
          }
        }
        if ((!is_dc))
          sim.push_back(step);
        if (is_dc)
          res += analy + line.mid(1, 2) + QString(" (name: ") + left_list[0] + QString(" ;Start: ") + left_list[1] + QString("; End:") + left_list[2] + QString("; Step: ") + left_list[3] + QChar(')');
        else
          res += analy + line.mid(1, 2) + QString(" (name: ") + left_list[0] + QString(" ;Start: ") + left_list[2] + QString("; End:") + left_list[3] + QString("; Step: ") + left_list[1] + QChar(')');
        return res;
      }
    }

    else if (line.mid(0, 7).contains(QRegExp("^.[Pp]rint $"))) // print
    {
      cout << "解析到print" << endl;
      QString left = line.mid(7, -1);
      QStringList left_list = left.split(' ');
      int sz = left_list.size();
      if (sz != 2 && sz != 1)
        res += get_cur_line() + QStringLiteral("the print info length is illeague");
      else
      {
        if (sz == 1)
          check_print(unknow_print, left_list[0], res);
        else
          check_print(left_list[0], left_list[1], res);
      }
    }
    else
      res += unknow + get_cur_line() + QString(" raw: ") + line; //.开头的网表格式错误,而且无法解析错误
  }

  else
  { //判断 器件
    cout << "解析到器件" << endl;
    bool right = true;
    QStringList strlist = line.split(' ');
    bool is_two = true;
    double init_hz = 0;
    if (strlist.size() == 11 && strlist[3].contains(QRegExp("^pulse$", Qt::CaseInsensitive))) //进入到脉冲源
    {
      cout << "解析到脉冲源" << endl;
      PulseDevice elem;
      line = line.remove(QChar('('));
      line = line.remove(QChar(')'));
      strlist = line.split(' ');
      qDebug() << "strlist[0]" << strlist[0];
      bool is_v = strlist[0].contains(QRegExp("^[Vv][a-zA-Z]*\\d*$"));
      bool is_i = strlist[0].contains(QRegExp("^[Ii][a-zA-Z]*\\d*$"));
      qDebug() << "is_v" << is_v;
      qDebug() << "is_i" << is_i;
      if (!(is_v || is_i)) //和check_dev_name有些重复
      {
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("pulse source name format is not right")); //
        res += get_cur_line() + "pulse source name format is not right";
        return res;
      }
      bool check = check_dev_name(strlist[0], res, 0);

      if (check)
        elem.name = strlist[0];

      if (is_v)
        elem.type = 0;
      else
        elem.type = 1;
      for (int i = 1; i < 11; i++)
      {
        bool check;
        if (i == 1 || i == 2)
        {
          check = strlist[i].contains(QRegExp("^[+-]?\\d+$"));
          check = check || strlist[i].contains(QRegExp("^[a-zA-Z]+_?\\d*$", Qt::CaseInsensitive)); //如何实现_可有可无，但是如果有_，后面必须得有数字 node不一定是数字
          if (check)
          { //这个节点之前没有碰到过
            string tmp = strlist[i].toStdString();
            if (!node_map.count(tmp))
            { // to do 维持映射：比如说是node1,然后它在matrix里是代表第i行，那么就是node1:i 先来的先占 node2row string int
              node2row[tmp] = Node_nums;
              qDebug() << " pulse node2row:" << strlist[i] << '\t' << Node_nums;
              node_map[tmp] = 1;
              Node_nums++;
            }
          }
          else
          {
            res += get_cur_line() + " pulse Node name is illegle ";
            QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("pulse Node name is illegle")); //
            return res;
          }
        }
        else if (i == 3)
          continue;
        else
        {
          check = strlist[i].contains(QRegExp("^\\d+(\\.\\d+)?$"));
          if (!check)
          {
            res += get_cur_line() + " pulse Node value is illegle ";
            QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("pulse Node value is illegle")); //
            return res;
          }
        }
      }
      dev_map[strlist[0].toStdString()] = 1;
      elem.nd1 = node2row[strlist[1].toStdString()];
      elem.nd2 = node2row[strlist[2].toStdString()];
      elem.start_value = qstring2double(strlist[4]);
      elem.pulse_value = qstring2double(strlist[5]);
      elem.start_time = qstring2double(strlist[6]);
      elem.tr = qstring2double(strlist[7]);
      elem.tf = qstring2double(strlist[8]);
      elem.pw = qstring2double(strlist[9]);
      elem.per = qstring2double(strlist[10]);
      elem.branch_idx = branch_sz;

      branch_sz++;
      // Device_nums++; //电压源不属于器件/元件
      V_nums++;

      elem.show_info();
      pulse_dev_list.push_back(elem);
      res += "lazy to parse pulse device";
      return res;
      // if (!((node2row.count(strlist[1].toStdString())) && (node2row.count(strlist[2].toStdString()))))
      // {
      //   QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("pulse node name not existed")); //
      //   res += get_cur_line() + "pulse node name not existed";
      //   return res;
      // }
    }

    right = check_dev_form(strlist, res, 0, sim, is_two, node2row, init_hz);
    //判断元件name 是否合法 R或者V开头，之后接字母和数字，数字要在最后
    if (right)
    {
      // cout << "解析到器件成功" << endl;
      string tmp = strlist[0].toStdString();
      dev_map[tmp] = 1;
      Device_nums++;
      QString name;
      // r c l v i 0-4  l v branch_sz++
      if (!is_two)
      {
        res += "FourPortDevice,lazy to parse it";
        FourPortDevice dev_elem;
        if (init_hz != 0)
          dev_elem.init_hz = init_hz;
        dev_elem.name = strlist[0];

        dev_elem.nd1 = node2row[strlist[1].toStdString()];
        dev_elem.nd2 = node2row[strlist[2].toStdString()];
        dev_elem.ncd1 = node2row[strlist[3].toStdString()];
        dev_elem.ncd2 = node2row[strlist[4].toStdString()];
        dev_elem.value = qstring2double(strlist[5]);
        if (strlist[0][0] == QChar('E') || strlist[0][0] == QChar('e'))
        {
          dev_elem.branch_idx = branch_sz;
          branch_sz++;
          dev_elem.type = 1;
        }
        else
        {
          dev_elem.type = 0;
        }
        qDebug() << "dev::::::::::" << dev_elem.name << dev_elem.nd1 << dev_elem.nd2 << dev_elem.ncd1 << dev_elem.ncd2 << dev_elem.value;
        four_dev_list.push_back(dev_elem);
        return res;
      }
      TwoPortDevice dev_elem;
      if (init_hz != 0)
        dev_elem.init_hz = init_hz;
      dev_elem.name = strlist[0];
      dev_elem.nd1 = node2row[strlist[1].toStdString()];
      dev_elem.nd2 = node2row[strlist[2].toStdString()];
      qDebug() << dev_elem.name << dev_elem.nd1 << dev_elem.nd2;
      dev_elem.value = qstring2double(strlist[3]);
      if (strlist[0][0] == QChar('V') || strlist[0][0] == QChar('v'))
      {
        dev_elem.branch_idx = branch_sz;
        branch_sz++;
        dev_elem.type = 3;
        Device_nums--; //电压源不属于器件/元件
        V_nums++;
        name = Vol;
      }
      else if (strlist[0][0] == QChar('I') || strlist[0][0] == QChar('i'))
      {
        dev_elem.type = 4;
        Device_nums--; //电流源不属于器件/元件
        name = I;
        I_nums++;
      }
      else if (strlist[0][0] == QChar('R') || strlist[0][0] == QChar('r'))
      {
        dev_elem.type = 0;
        R_nums++;
        name = reg;
      }
      else if (strlist[0][0] == QChar('L') || strlist[0][0] == QChar('l'))
      {
        dev_elem.branch_idx = branch_sz;
        branch_sz++;
        dev_elem.type = 2;
        name = L;
        L_nums++;
      }
      else
      {
        dev_elem.type = 1;
        name = cap;
        C_nums++;
      }
      two_dev_list.push_back(dev_elem);
      res += name + dev_info(strlist); //四端器件就不解析了
    }
    else
      res += QString("   raw: ") + line;
  }
  return res;
}
QString file_parse::get_and_parse_one_line(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran)
{
  // QString a = get_next_line();
  return parse_one_line(get_next_line(), two_dev_list, four_dev_list, pulse_dev_list, branch_sz, sim, node2row, plot, tran);
}
int file_parse::get_dev_num() //为了给矩阵提前分配空间
{
  return dev_map.size();
}
int file_parse::get_node_num() //为了给矩阵提前分配空间
{
  return node_map.size();
}
QString file_parse::parse(vector<TwoPortDevice> &two_dev_list, vector<FourPortDevice> &four_dev_list, vector<PulseDevice> &pulse_dev_list, int &sum_sz, int &branch_sz, vector<double> &sim, unordered_map<string, int> &node2row, vector<int> &plot, vector<double> &tran, circuit *circuit)
{
  QString tmp;
  QString res;
  // cout << 6 << endl;
  qDebug() << "before" << get_dev_num() << '\t' << get_node_num();
  // int num = 0;每10倍频10个点
  do
  {
    // cout << "1";
    // num++;
    tmp = get_and_parse_one_line(two_dev_list, four_dev_list, pulse_dev_list, branch_sz, sim, node2row, plot, tran);
    if (tmp != NULL)
    {
      // if (num < 10)
      //   cout << tmp.toStdString();
      // cout << 111 << endl; //不加endl打印不出来
      res += tmp + "\n";
    }
  } while (tmp != NULL);
  show_dev_nums(res);
  parsed_res = res;
  // qDebug() << "after" << get_dev_num() << '\t' << get_node_num();
  qDebug() << 666;
  for (auto &x : sim)
    qDebug() << x << '\t';
  qDebug() << 666;
  sum_sz = branch_sz + node_map.size();
  qDebug() << circuit->sum_sz << 666666666;
  return res;
}

// QString fileName = QFileDialog::getOpenFileName( 0,"TB1.sp", "./testbench/");//打开文件并且保存名字
// if(fileName.isEmpty() == true){
//   cout<<"选择文件无效!"<<endl;
//     return -1;
// }
// // ui->lineEdit_file->setText(fileName);//给编辑框显示文件名
// //创建文件对象
// QFile file(fileName);
// //指定打开方式 open
// bool isOk = file.open(QFile::ReadWrite);//以读写方式打开，可以only read，only write
// if(isOk == false){
//     cout<<"open file failed!"<<endl;
//     return -1;
// }

// while(!file.atEnd())
// {
//     QString qstring = file.readLine();
//     // ui->textEdit_file->append(string);
//     string value = qstring.toStdString();
//     cout<<value<<endl;
//     // qDebug() << value << endl;
// }
// file.close();//没有返回值