#include "qcustomplot.h"
#include "plotter.h"
#include <QApplication>
#include <QDebug>
#include <QString>
#include <QVector>

void plotter::plot_it(QVector<double> &xdata, QVector<double> &ydata, QString xlabel, QString ylabel, int is_ac)
{
    // todo指数
    QCustomPlot *custPlot = new QCustomPlot();
    custPlot->addGraph(custPlot->xAxis, custPlot->yAxis);
    custPlot->graph(0)->setPen(QPen(QPen(Qt::blue)));
    custPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    custPlot->graph(0)->setData(xdata, ydata);
    custPlot->graph(0)->rescaleAxes();
    custPlot->xAxis->setLabel(xlabel);
    custPlot->yAxis->setLabel(ylabel);
    // custPlot->replot();
    custPlot->graph(0)->setName(ylabel);
    custPlot->legend->setVisible(true);
    custPlot->legend->setFont(QFont("Helvetica", 15));
    custPlot->setWindowTitle("graph");
    custPlot->setMinimumSize(400, 300);
    if (is_ac == 1)
    {
        QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
        custPlot->xAxis->setTicker(logTicker);
        custPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    }
    custPlot->show();

    return;
}