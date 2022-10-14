#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "subwidget.h"
#include "parse.h"
#include <QMainWindow>
class QWidget;
class QAction;
class QMenu;
class QToolBar;
class QTextEdit; //？不是include
class QFile;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initActions();
    void initMenus();
    void initToolbars();

public slots:
    void slotOpenFile();
    void slotParseNetList();
    void slotParse();

private:
    QFile *m_file;
    QAction *m_fileOpenAction;
    QMenu *m_fileMenu;
    QToolBar *m_Parsebar; //为什么是指针？
    QToolBar *m_fileToolbar;
    QString m_fileName;
    QTextEdit *m_textEdit;
    QAction *m_parseAction; // parseAction demo
    // SubWidget *m_subWidget;
};

#endif // MAINWINDOW_H