#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "subwidget.h"
#include "circuit/circuit.h" //?
#include "parse/parse.h"
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
class QWidget;
class QAction;
class QMenu;
class QToolBar;
class QTextEdit; //？不是include
class QFile;
class QDockWidget;
class QIcon;
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
    void slotSaveFile();

private:
    circuit *m_circuit;
    QFile *m_file;
    QAction *m_fileOpenAction;
    QAction *m_fileSaveAction;
    QMenu *m_fileMenu;
    QToolBar *m_Parsebar; //为什么是指针？
    QToolBar *m_Matrixbar;
    QToolBar *m_Plotbar;
    QToolBar *m_fileToolbar;
    QToolBar *m_fileSavebar;
    QString m_fileName;

    QWidget *main_widget;

    QTextEdit *m_raw_text;
    QTextEdit *m_matrix_text;
    QTextEdit *m_parse_text;
    QSplitter *m_Hsplitter;
    QSplitter *m_Hsplitter_matrix;
    QSplitter *m_main_splitter;
    QHBoxLayout *main_layout;

    QAction *m_parseAction;  // parseAction demo
    QAction *m_MatrixAction; // parseAction demo
    QAction *m_PlotAction;   // parseAction demo
    // SubWidget

    // SubWidget *m_subWidget;
    // QDockWidget *m_bottom_dock_window;
};

#endif // MAINWINDOW_H