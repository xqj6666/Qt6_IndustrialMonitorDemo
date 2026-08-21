#include "mainwindow.h"
#include "pages/monitorpage.h"
#include "pages/chartpage.h"
#include "pages/configpage.h"
#include "core/logger.h"

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
    m_logView->setStyleSheet(
        "QTextEdit {"
        "    background-color: #1e1e1e;"   // 深色背景
        "    color: #dcdcdc;"              // 默认文字颜色
        "    font-family: Consolas, monospace;"  // 等宽字体，日志对齐好看
        "    font-size: 12px;"
        "    border: 1px solid #333;"
        "}"
        );
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
    //连接日志信号到日志显示的槽函数
    connect(Logger::instance(),&Logger::logMessageReady,this,[this](const QString &msg, Logger::Level level){
        onLogMessage(msg,static_cast<int>(level));
    });//信号发送的是枚举类型，槽函数接收的是int，需要进行转换
}

void MainWindow::initData()
{
    //程序启动日志
    Logger::info("程序启动");
    Logger::info("界面初始化完成");
}

void MainWindow::onLogMessage(const QString &formattedMsg, int level)
{
    // 根据日志级别设置颜色
    switch (static_cast<Logger::Level>(level)) {
    case Logger::Level::Debug:
        m_logView->setTextColor(QColor(160, 160, 160));  // 灰色
        break;
    case Logger::Level::Info:
        m_logView->setTextColor(QColor(220, 220, 220));  // 白色
        break;
    case Logger::Level::Warning:
        m_logView->setTextColor(QColor(255, 200, 0));    // 黄色
        break;
    case Logger::Level::Error:
        m_logView->setTextColor(QColor(255, 80, 80));    // 红色
        break;
    }

    m_logView->append(formattedMsg);

    // 写完后滚动到底部
    QTextCursor cursor = m_logView->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logView->setTextCursor(cursor);
}



