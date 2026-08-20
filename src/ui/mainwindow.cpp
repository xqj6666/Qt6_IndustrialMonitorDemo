#include "mainwindow.h"
#include "pages/monitorpage.h"
#include "pages/chartpage.h"
#include "pages/configpage.h"

#include <QTabWidget>
#include <QStatusBar>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    initConnect();
    initData();
}

MainWindow::~MainWindow()
{
    // 对象树自动析构，这里一般不需要手动 delete
}

void MainWindow::initUI()
{
    setWindowTitle("工业监控上位机 v1.0");
    resize(1200, 800);

    //-----主分割器-----
    m_mainSplitter = new QSplitter(Qt::Vertical,this);//水平分割，可以嵌套
    setCentralWidget(m_mainSplitter);

    //-----上半部分:Tab多页面容器-----
    m_tabWidget = new QTabWidget(m_mainSplitter);
    m_mainSplitter->addWidget(m_tabWidget);
    m_tabWidget->addTab(new MonitorPage(m_tabWidget),"设备监控");
    m_tabWidget->addTab(new ChartPage(m_tabWidget),"数据曲线");
    m_tabWidget->addTab(new ConfigPage(m_tabWidget),"参数配置");


    //-----下半部分：日志显示区域-----
    m_logView = new QTextEdit(m_mainSplitter);
    m_logView->setReadOnly(true);//只读
    m_mainSplitter->addWidget(m_logView);
    m_mainSplitter->setSizes({700, 300});
    m_mainSplitter->setStretchFactor(0,7);//上班部分占比7
    m_mainSplitter->setStretchFactor(1,3);//下半部分占比3

    //-----状态栏-----
    m_statusLabel = new QLabel("就绪",this);
    statusBar()->addPermanentWidget(m_statusLabel);
}

void MainWindow::initConnect()
{
    // 后面逐步添加信号槽连接
}

void MainWindow::initData()
{
    // 后面加载 ini 配置、初始化数据
}