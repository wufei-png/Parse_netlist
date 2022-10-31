
#include <QTextEdit>
#include <QTextStream>
#include "subwidget.h"
SubWidget::SubWidget()
{
  setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
  hide();
}

SubWidget::~SubWidget()
{
}
