#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "subwidget.h"
#include "circuit/circuit.h" //?
#include "parse/parse.h"
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
    void slotMatrixNetList();
    void slotPlotNetList();

private:
    circuit *m_circuit;
    QFile *m_file;
    QAction *m_fileOpenAction;
    QMenu *m_fileMenu;
    QToolBar *m_Parsebar; //为什么是指针？
    QToolBar *m_Matrixbar;
    QToolBar *m_Plotbar;
    QToolBar *m_fileToolbar;
    QString m_fileName;
    QTextEdit *m_textEdit;
    QAction *m_parseAction;  // parseAction demo
    QAction *m_MatrixAction; // parseAction demo
    QAction *m_PlotAction;   // parseAction demo
    // SubWidget *m_subWidget;
};

#endif // MAINWINDOW_H