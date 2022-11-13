#include "mainwindow.h"
#include "util/add.h"
#include "util/tool.h"
#include <QAction>
#include <QDockWidget>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QIcon>
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
    qDebug() << "wf";
    main_widget = new QWidget(this);

    m_raw_text = new QTextEdit(main_widget);

    m_parse_text = new QTextEdit(main_widget);
    m_parse_text->setReadOnly(true);
    m_parse_text->append("parse output info:"); //自动换行？

    m_matrix_text = new QTextEdit(main_widget);
    m_matrix_text->setReadOnly(true);
    m_matrix_text->append("matrix output info:");

    main_layout = new QHBoxLayout();
    // main_layout->addWidget(m_raw_text);
    // main_layout->addWidget(m_parse_text);
    // main_layout->setStretchFactor(m_raw_text, 2);
    // main_layout->setStretchFactor(m_parse_text, 2);

    m_main_splitter = new QSplitter(Qt::Vertical, main_widget);
    m_Hsplitter = new QSplitter(Qt::Horizontal, m_main_splitter);
    m_Hsplitter_matrix = new QSplitter(Qt::Horizontal, m_main_splitter);

    m_Hsplitter->addWidget(m_raw_text);
    m_Hsplitter->addWidget(m_parse_text);

    // m_Vsplitter->addWidget(m_Hsplitter);
    m_Hsplitter_matrix->addWidget(m_matrix_text);

    // splitter->setStretchFactor(0, 1);
    // splitter->setStretchFactor(1, 1);

    m_main_splitter->addWidget(m_Hsplitter);
    m_main_splitter->addWidget(m_Hsplitter_matrix);

    main_layout->addWidget(m_main_splitter);

    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);
    qDebug() << 123123;
    // m_bottom_dock_window = new QDockWidget("parse result", this);
    // m_subWidget = new SubWidget(m_bottom_dock_window);
    // // m_subWidget->move(250, 250);
    // // m_subWidget->show();
    // m_subWidget->show();
    // m_bottom_dock_window->setWidget(m_subWidget);
    // setCentralWidget(m_subWidget);
    //  dock->layout->addWidget(m_subWidget);
    //  QHBoxLayout *layout = new QHBoxLayout(m_subWidget);
    //  layout->addWidget(m_subWidget);

    // addDockWidget(Qt::BottomDockWidgetArea, m_bottom_dock_window);
    // dock->setAnimated(1);
    // setAnimated(1);
    // resize(800, 600);
    setWindowTitle(tr("MainWindow"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::initActions()
{

    QIcon icon;
    m_parseAction = new QAction(tr("parsed"), this);
    m_parseAction->setToolTip(tr("use parsed to parse netlist"));
    connect(m_parseAction, SIGNAL(triggered()), this, SLOT(slotParseNetList())); //菜单
    icon.addFile(QString("./src/images/parse.jpg"));
    // If fileName contains a relative path (e.g. the filename only) the relevant file must be found relative to the runtime working directory.
    m_parseAction->setIcon(icon);

    m_MatrixAction = new QAction(tr("Matrix"), this);
    m_MatrixAction->setToolTip(tr("use Matrix to calculate the parsed netlist"));
    connect(m_MatrixAction, SIGNAL(triggered()), this, SLOT(slotMatrixNetList()));
    icon.addFile(QString("./src/images/matrix.jpg"));
    m_MatrixAction->setIcon(icon);

    m_fileOpenAction = new QAction(tr("open file"), this);
    m_fileOpenAction->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(m_fileOpenAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
    QIcon icon_file; //这个不能复用。。。。
    icon_file.addFile(QString("./src/images/file.jpg"));
    m_fileOpenAction->setIcon(icon_file);

    m_fileSaveAction = new QAction(tr("save file"), this);
    connect(m_fileSaveAction, SIGNAL(triggered()), this, SLOT(slotSaveFile()));

    m_PlotAction = new QAction(tr("Plot"), this);
    m_PlotAction->setToolTip(tr("use Plotter to Plot"));
    connect(m_PlotAction, SIGNAL(triggered()), this, SLOT(slotPlotNetList()));
    icon.addFile(QString("./src/images/plot.jpg"));
    m_PlotAction->setIcon(icon);
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

    m_fileSavebar = addToolBar(tr("File Save"));
    m_fileSavebar->addAction(m_fileSaveAction);

    m_Parsebar = addToolBar(tr("Parse")); //这个不显示？？？
    m_Parsebar->addAction(m_parseAction);

    m_Matrixbar = addToolBar(tr("Matrix"));
    m_Matrixbar->addAction(m_MatrixAction);

    m_Plotbar = addToolBar(tr("Plot"));
    m_Plotbar->addAction(m_PlotAction);
}
void MainWindow::slotSaveFile()
{
    if (m_fileName.isEmpty())
        QMessageBox::warning(this, tr("Error"), tr("have not opened one file!")); // this还是NULL
    else
    {

        // strText.toLatin1();
        QString strText = m_raw_text->toPlainText();
        // m_file->flush();
        m_file->seek(0); //
        qDebug() << "rawtext" << strText;
        int write_len = m_file->write(strText.toLatin1());
        m_file->seek(0); //
        m_file->resize(strText.length());
        qDebug() << write_len;
    }
}
void MainWindow::slotParseNetList()
{
    qDebug() << "slotParseNetList";
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
        m_parse_text->append(text);
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
    qDebug() << "slotMatrixNetList";
    if (m_fileName.isEmpty())
        QMessageBox::warning(this, tr("Error"), tr("have not opened one file!")); // this还是NULL
    else
        m_matrix_text->append(m_circuit->get_matrix_res());
}

void MainWindow::slotPlotNetList()
{
    qDebug() << "slotPlotNetList";
    if (m_fileName.isEmpty())
        QMessageBox::warning(this, tr("Error"), tr("have not opened one file!")); // this还是NULL
    else
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
    if (!m_file->open(QIODevice::ReadWrite | QIODevice::Text))
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
                m_raw_text->setPlainText(textStream.readAll());
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