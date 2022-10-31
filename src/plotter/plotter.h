#ifndef PLOTTER_H
#define PLOTTER_H
#include <QString>
#include <QDebug>
#include <QVector>
#include <complex>
#include <iostream>


using namespace std;
struct plotter
{
public:
  void plot_it(QVector<double> &xdata, QVector<double> &ydata, QString xlabel, QString ylabel, int is_ac);
  plotter();
  ~plotter();
};
#endif // PLOTTER_H