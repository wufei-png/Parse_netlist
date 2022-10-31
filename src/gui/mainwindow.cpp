#include "mainwindow.h"
#include "util/add.h"
#include <QAction>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextStream>
#include <QToolBar>

#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) //?
{
    m_file = new QFile(); //为简便就不弄initfile了
    initActions();
    initMenus();
    initToolbars();
    //为简便就不弄initsubwindow了
    // m_subWidget = new SubWidget();
    // m_subWidget->move(250, 250);
    // m_subWidget->show();
    m_textEdit = new QTextEdit(this);
    setCentralWidget(m_textEdit);

    resize(800, 600);
    setWindowTitle(tr("MainWindow"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::initActions()
{

    m_parseAction = new QAction(tr("parsed"), this);
    m_parseAction->setToolTip(tr("use parsed to parse netlist"));
    connect(m_parseAction, SIGNAL(triggered()), this, SLOT(slotParseNetList())); //菜单

    m_MatrixAction = new QAction(tr("Matrix"), this);
    m_MatrixAction->setToolTip(tr("use Matrix to calculate the parsed netlist"));
    connect(m_MatrixAction, SIGNAL(triggered()), this, SLOT(slotMatrixNetList()));

    m_PlotAction = new QAction(tr("Plot"), this);
    m_PlotAction->setToolTip(tr("use Plotter to Plot"));
    connect(m_PlotAction, SIGNAL(triggered()), this, SLOT(slotPlotNetList()));

    m_fileOpenAction = new QAction(tr("open file"), this);
    m_fileOpenAction->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(m_fileOpenAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
}

void MainWindow::initMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("File"));
    m_fileMenu->addAction(m_fileOpenAction);
}

void MainWindow::initToolbars()
{
    m_fileToolbar = addToolBar(tr("File"));
    m_fileToolbar->addAction(m_fileOpenAction);

    m_Parsebar = addToolBar(tr("Parse")); //这个不显示？？？
    m_Parsebar->addAction(m_parseAction);

    m_Matrixbar = addToolBar(tr("Matrix"));
    m_Matrixbar->addAction(m_MatrixAction);

    m_Plotbar = addToolBar(tr("Plot"));
    m_Plotbar->addAction(m_PlotAction);
}
void MainWindow::slotParseNetList()
{
    // cout << 666;
    // m_circuit = new circuit(m_file);
    QString text;
    cout << m_file;
    if (m_fileName.isEmpty())
        QMessageBox::warning(this, tr("Error"), tr("have not opened one file!")); // this还是NULL
    else
    {
        // m_circuit = new circuit(m_file);
        text = m_circuit->get_parse_res();
        m_textEdit->append(text);
    }

    // label->setText(text);
    // QFont font("Microsoft YaHei", 10, 75); //第一个属性是字体（微软雅黑），第二个是大小，第三个是加粗（权重是75）
    // label->setFont(font);
    // /** @brief Detail settings for label.
    //  * You can find more functions in ref web at the front of this file */
    // label->setAlignment(Qt::AlignCenter);
    // // label->setStyleSheet("QLabel{font:15px;color:red;background-color:rgb(f9,f9,"
    // //  "f9);}");
    // // label->setStyleSheet("font:30px;color:black;background-color:yellow");
    // label->resize(800, 600);
    // label->show(); // label should be shown to be seen.
}
void MainWindow::slotMatrixNetList()
{
    m_textEdit->append(m_circuit->get_matrix_res());
}

void MainWindow::slotPlotNetList()
{
    qDebug() << "plot";
    m_circuit->plot_res();
}

void MainWindow::slotOpenFile()
{
    qDebug() << "slotOpenFile() --";

    m_fileName = QFileDialog::getOpenFileName(this, tr("Open File"), tr(""), "Text File (*.sp)");
    qDebug() << "open file " << m_fileName;

    if (m_fileName.isEmpty())
        return;

    m_file->setFileName(m_fileName);
    if (!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open file!"));
        return;
    }
    else
    {
        cout << "yes";
        if (!m_file->isReadable())
        {
            QMessageBox::warning(this, tr("Error"), tr("The file is unreadable"));
        }
        else
        {
            // m_circuit = new circuit(m_file); //为什么在这里不可以?????? bug
            QTextStream textStream(m_file); // Use QTextStream to load text.
            while (!textStream.atEnd())
            {
                // qDebug() << textStream.readAll();
                m_textEdit->setPlainText(textStream.readAll());
            }
            m_circuit = new circuit(m_file); //重新打开一个，要重新new
            // file.close();
        }
    }

    int a = 10, b = 5;

    int sum = add(a, b);

    qDebug() << "sum = " << sum;
}

void MainWindow::slotParse()
{
    // m_fileName;
}