
#include "subwidget.h"
SubWidget::SubWidget(QWidget *parent) : QWidget(parent)
{
  // setWindowFlags(Qt::CustomizeWindowHint | Qt::SubWindow);
  // hide();
  m_textEdit = new QTextEdit(this);
  // connect 监控dock的大小变化
  m_textEdit->resize(parent->size());
  // QMainWindow::setCentralWidget(m_textEdit);
  // // m_textEdit->setText("1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n11\n");
  // qDebug() << "12312312312312313";
  hide();

  // resize(expand)
}

SubWidget::~SubWidget()
{
}
