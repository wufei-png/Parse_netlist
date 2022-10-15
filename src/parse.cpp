/*
  file_parse
  */
/*

功能：只能解析一行的数据完整的情形


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


功能问题
会有.DC I 0.5 2 0.1也能解析对的尴尬局面

遇到的问题和知识点：
没有换行符不能打印，Qdebug不需要换行，因为qdebug自带换行。

正则大小写[oO]   ||   setCaseSensitivity（CaseInsensitive//0）
if嵌套太多

增加删减文件要修改pro
如果起了Qapplication，那么cout必须要endl才能输出


*/

#include "parse.h"
using namespace std;

QString file_parse::get_cur_line()
{
  return QString(" line ") + QString(to_string(parsed_nums).c_str()) + QString(": ");
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
bool file_parse::check_dev_name(QString &line, QString &res, bool simulation)
{
  const QRegExp dev_rep("^[CcRVrvLl][a-z]*\\d*$"); // r c l
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
bool file_parse::check_dev_value(QStringList list, bool simulation) // sim参数代表不是dc模拟的 前两个是节点号，不应该含k或者小数点
{
  bool check;
  for (int i = 1; i < 4; i++)
  {
    if (!simulation && i != 3) // node name  可以改进：如果是模拟情况，这个节点必须已经在哈希表中了
    {
      check = list[i].contains(QRegExp("^[+-]?\\d+$"));
      if (check)
      { //这个节点之前没有碰到过
        string tmp = list[i].toStdString();
        if (!node_map.count(tmp))
        {
          node_map[tmp] = 1;
          Node_nums++;
        }
      }
    }
    else // digit
    {
      check = list[i].contains(QRegExp("^[+-]?\\d+(\\.\\d+)?k?$"));        //都是数字,带正负，可以带k 可以是小数
      check = check || list[i].contains(QRegExp("^[+-]?\\d+e[+-]?\\d+$")); //带e的科学计数
    }

    if (!check)
      return false;
  }
  return true;
}
bool file_parse::check_dev_form(QStringList &strlist, QString &res, bool sim)
{
  bool right = true;
  if (strlist.size() != 4)
  {
    res += get_cur_line() + QStringLiteral("device parameter length is not right");
    return false;
  }
  // cout << "器件名称没问题123" << endl;
  if (!check_dev_name(strlist[0], res, sim))
  {
    // res += QString("原件命名有问题");
    // cout << "bugbug1";
    right = false;
  }
  // cout << "器件名称没问题" << endl;
  if (!check_dev_value(strlist, sim))
  {
    res += get_cur_line() + QStringLiteral("device value error");
    right = false;
  }
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
QString file_parse::parse_one_line(const QString line)
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
    parsed_nums--;                 //用正常逻辑，其他的加，要if嵌套或者很多情况都要写了
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
      right = check_dev_form(left_list, res, 1);
      if (right)
      {
        res += analy + line.mid(1, 2) + QString(" (name: ") + left_list[0] + QString(" ;Start: ") + left_list[1] + QString("; End:") + left_list[2] + QString("; Step: ") + left_list[3] + QChar(')');
        return res;
      }
    }
    else if (line.mid(0, 7).contains(QRegExp("^.[Pp]rint $"))) // print
    {
      cout << "解析到print" << endl;
      QString left = line.mid(7, -1);
      QStringList left_list = left.split(' ');
      if (left_list.size() != 2)
        res += get_cur_line() + QStringLiteral("the print info length is not 2");
      else
      {
        if (!left_list[0].contains(QRegExp("^[aADd][Cc]$")))
          res += get_cur_line() + QStringLiteral("cannot find print info device type");
        else if (!left_list[1].contains(QRegExp("^[IiVv]\\(\\d+\\)$")))
          res += get_cur_line() + QStringLiteral("the print info's value is illegal");
        else //正常
        {
          res += out_info + left_list[0] + QString("; Type:") + left_list[1][0] + QChar(' ') + left_list[1].mid(2, left_list[1].size() - 3) + QChar(')');
        }
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
    right = check_dev_form(strlist, res, 0);
    //判断元件name 是否合法 R或者V开头，之后接字母和数字，数字要在最后
    if (right)
    {
      // cout << "解析到器件成功" << endl;
      string tmp = strlist[0].toStdString();
      dev_map[tmp] = 1;
      Device_nums++;
      QString name;
      if (strlist[0][0] == QChar('V') || strlist[0][0] == QChar('v'))
      {
        Device_nums--; //电压源不属于设备
        V_nums++;
        name = Vol;
      }

      else if (strlist[0][0] == QChar('R') || strlist[0][0] == QChar('r'))
      {
        R_nums++;
        name = reg;
      }
      else if (strlist[0][0] == QChar('L') || strlist[0][0] == QChar('l'))
      {
        name = L;
        L_nums++;
      }
      else
      {
        name = cap;
        C_nums++;
      }

      res += name + dev_info(strlist);
    }
    else
      res += QString("   raw: ") + line;
  }
  return res;
}
QString file_parse::get_and_parse_one_line()
{
  // QString a = get_next_line();
  return parse_one_line(get_next_line());
}

QString file_parse::parse()
{
  QString tmp;
  QString res;
  cout << 6 << endl;
  // int num = 0;
  do
  {
    // cout << "1";
    // num++;
    tmp = get_and_parse_one_line();
    if (tmp != NULL)
    {
      // if (num < 10)
      //   cout << tmp.toStdString();
      // cout << 111 << endl; //不加endl打印不出来
      res += tmp + " \n";
    }
  } while (tmp != NULL);
  show_dev_nums(res);
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