#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QWidget>

class QWidget;
class QAction;
class QMenu;
class QToolBar;
class QTextEdit;

class SubWidget : public QWidget
{
public:
    SubWidget(); //？
    ~SubWidget();

public slots:
    void slotOpenFile();
    void slotParse();

private:
    QTextEdit *m_textEdit;
};

#endif // SUBWINDOW_H