//===主窗口界面===
#include "mainwindow.h"
#include "pages/monitorpage.h"
#include "pages/chartpage.h"
#include "pages/configpage.h"
#include "core/logger.h"
#include "communication/modbusclient.h"

#include <QTabWidget>
//不要自己new QStatusBar，QMainWindow内置示例，直接调用statusBar()
#include <QStatusBar>
#include <QLabel>
#include <QSplitter>
#include <QTextEdit>
#include <QThread>
//属于Qt元对象系统，让Qt信号-槽、QVariant可以自定义识别数据类型
#include <QMetaType>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //注册自定义类型，让信号槽跨线程传递时Qt能识别
    //mainwindow天然include了modbusclient，所以信号不再传递int，而是直接传递ModbusClient::DeviceState，语义清晰
    qRegisterMetaType<ModbusClient::DeviceState>("ModbusClient::DeviceState");
    qRegisterMetaType<QVector<quint16>>("QVector<quint16>");

    initUI();
    //如果initConnect先调用,对于m_modbusClient的连接会有空指针崩溃
    initCommunication();
    initConnect();
    initData();
}

MainWindow::~MainWindow()
{
    //先通知ModbusClient停止工作
    emit requestDisconnect();

    //停止子线程
    if(m_commThread)
    {
        m_commThread->quit();//退出事件循环
        m_commThread->wait();//等待子线程真正结束
    }
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
    //保存页面指针，后面connect要用
    m_monitorPage = new MonitorPage(m_tabWidget);
    m_configPage  = new ConfigPage(m_tabWidget);
    m_chartPage   = new ChartPage(m_tabWidget);

    m_tabWidget->addTab(m_monitorPage,"设备监控");
    m_tabWidget->addTab(m_chartPage,"数据曲线");
    m_tabWidget->addTab(m_configPage,"参数配置");

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
    //直接通过statusBar调用函数，改变最下方的状态栏
    statusBar()->addPermanentWidget(m_statusLabel);
}

void MainWindow::initCommunication()
{
    //创建通信子线程
    m_commThread = new QThread(this);
    m_modbusClient = new ModbusClient();

    //现成启动后,触发ModbusClient::init()在子线程里创建对象
    connect(m_commThread, &QThread::started, m_modbusClient, &ModbusClient::init);

    //把ModbusClient搬到子线程
    m_modbusClient->moveToThread(m_commThread);

    //线程结束后自动销毁ModbusClient
    connect(m_commThread,&QThread::finished,m_modbusClient,&QObject::deleteLater);

    //启动子线程
    m_commThread->start();
}


void MainWindow::initConnect()
{
    // ===== Logger单例 -> MainWindow：接收日志消息 =====
    connect(Logger::instance(), &Logger::logMessageReady, this,
            [this](const QString &msg, Logger::Level level) {
                onLogMessage(msg, static_cast<int>(level));
            });

    // ===== MainWindow -> ModbusClient：下发操作命令 原生connect优先 =====
    connect(this, &MainWindow::requestConnect,        m_modbusClient, &ModbusClient::connectToDevice);
    connect(this, &MainWindow::requestDisconnect,     m_modbusClient, &ModbusClient::disconnectDevice);
    connect(this, &MainWindow::requestStartPolling,   m_modbusClient, &ModbusClient::startPolling);
    connect(this, &MainWindow::requestStopPolling,    m_modbusClient, &ModbusClient::stopPolling);

    // ===== ModbusClient -> MainWindow：上报状态、寄存器数据、错误 =====
    // 设备状态变更，枚举转int
    connect(m_modbusClient, &ModbusClient::deviceStateChanged, this,
            [this](ModbusClient::DeviceState state, const QString &ip, quint16 port) {
                onDeviceStateChanged(static_cast<int>(state), ip, port);
            });

    // 寄存器数据上报，中转到MonitorPage，增加空指针保护
    connect(m_modbusClient, &ModbusClient::registerDataReady, this,
            [this](int startAddr, const QVector<quint16> &values) {
                if(m_monitorPage){
                    m_monitorPage->onRegisterDataReady(startAddr, values);
                }
                if(m_chartPage){
                    m_chartPage->onRegisterDataReady(startAddr, values);
                }
            });

    // Modbus错误，转发日志静态接口
    connect(m_modbusClient, &ModbusClient::errorOccurred, this,
            [](const QString &errorMsg) {
                Logger::error(errorMsg);
            });

    // ===== ModbusClient -> Logger：modbus内部日志消息转发 =====
    connect(m_modbusClient, &ModbusClient::logMessage, this,
            [](const QString &msg, int level) {
                switch (static_cast<Logger::Level>(level))
                {
                case Logger::Level::Debug:      Logger::debug(msg);     break;
                case Logger::Level::Info:       Logger::info(msg);      break;
                case Logger::Level::Warning:    Logger::warning(msg);   break;
                case Logger::Level::Error:      Logger::error(msg);     break;
                }
            });

    // ===== MonitorPage -> MainWindow：页面按钮操作向上通知主窗口 =====
    connect(m_monitorPage, &MonitorPage::connectRequested,    this, &MainWindow::onConnectRequested);
    connect(m_monitorPage, &MonitorPage::disconnectRequested, this, &MainWindow::onDisconnectRequested);

    // ===== MainWindow -> UI页面联动：收到连接请求自动切换监控tab页 =====
    connect(this, &MainWindow::requestConnect, this,
            [this](const QString &ip, quint16 port) {
                Q_UNUSED(ip);
                Q_UNUSED(port);
                m_tabWidget->setCurrentWidget(m_monitorPage);
            });
}

void MainWindow::initData()
{
    //程序启动日志
    Logger::info("程序启动");
    Logger::info("界面初始化完成");
}


void MainWindow::onDeviceStateChanged(int state,const QString &ip,quint16 port)
{
    //转发给MonitorPage 更新指示灯和按钮状态
    m_monitorPage->onDeviceStateChanged(state, ip, port);

    switch(static_cast<ModbusClient::DeviceState>(state))
    {
    case ModbusClient::DeviceState::Disconnected:
        m_statusLabel->setText(QString("已断开"));
        Logger::warning(QString("设备断开：%1:%2").arg(ip).arg(port));
        break;
    case ModbusClient::DeviceState::Connecting:
        m_statusLabel->setText(QString("连接中：%1:%2").arg(ip).arg(port));
        break;
    case ModbusClient::DeviceState::Connected:
        m_statusLabel->setText(QString("已连接：%1:%2").arg(ip).arg(port));
        Logger::info(QString("设备连接成功：%1:%2").arg(ip).arg(port));
        break;
    case ModbusClient::DeviceState::Error:
        m_statusLabel->setText("连接异常");
        Logger::error(QString("设备连接异常：%1:%2").arg(ip).arg(port));
        break;
    }
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

void MainWindow::onConnectRequested()
{
    //从ConfigPage读取配置参数
    QString ip      = m_configPage->ipAddress();
    quint16 port    = m_configPage->port();
    int interval    = m_configPage->pollInterval();

    Logger::info(QString("用户发起连接：%1:%2，轮询周期：%3ms").arg(ip).arg(port).arg(interval));

    //发信号给子线程的ModbusClient
    emit requestConnect(ip, port);

    //连接成功后自动启动轮询,延迟500ms，等连接建立后再启动轮询,doPoll里有是否连接的前置检查。
    //connectDevice是异步的，如果还没有连接就开始轮询会浪费轮询次数
    QTimer::singleShot(500, this, [this, interval](){
        emit requestStartPolling(interval, 0, 10);
    });
}

void MainWindow::onDisconnectRequested()
{
    Logger::info("用户发起断开连接");

    //先停止轮询,再断开连接
    emit requestStopPolling();
    emit requestDisconnect();
}



