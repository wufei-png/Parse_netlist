#ifndef SUBWINDOW_H
#define SUBWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QDebug>
class QWidget;
class QAction;
class QMenu;
class QToolBar;
class QTextEdit;

class SubWidget : public QWidget
{
public:
    SubWidget(QWidget *parent); //？
    ~SubWidget();

private:
    QTextEdit *m_textEdit;
};

#endif // SUBWINDOW_H