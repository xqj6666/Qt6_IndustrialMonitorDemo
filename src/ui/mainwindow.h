#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QTabWidget;
class QSplitter;
class QTextEdit;
class QThread;
QT_END_NAMESPACE

class Logger;
class ModbusClient;
class MonitorPage;
class ConfigPage;
class ChartPage;
class QThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindow)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    //发给子线程ModbusClient的信号
    void requestConnect(const QString &ip, quint16 port);
    void requestDisconnect();
    void requestStartPolling(int intervalMs, int startAddr, int registerCount);
    void requestStopPolling();

private:
    void initUI();       // 初始化界面
    void initConnect();  // 初始化信号槽连接
    void initData();     // 初始化数据、加载配置
    void initCommunication(); //初始化通信层(子线程+ModbusClient)

private slots:
    void onLogMessage(const QString &formattedMsg,int level);//接受日志消息，前置什么无法访问类内部的枚举，用int做桥梁
    void onDeviceStateChanged(int state, const QString &ip, quint16 port);
    void onConnectRequested();//用户点击monitorpage的连接设备按钮
    void onDisconnectRequested();//用户点击monitorpage的断开连接按钮,既然monitorpage也在主线程，为什么不直接发信号给modbusclient???

private:
    QSplitter   *m_mainSplitter = nullptr;//主分割器(上下分割)
    QTabWidget  *m_tabWidget    = nullptr;//多页面容器
    QTextEdit   *m_logView      = nullptr;//日志显示区域
    QLabel      *m_statusLabel  = nullptr;//状态连信息标签

    //---页面指针---
    MonitorPage *m_monitorPage  = nullptr;//之前是直接new的，为什么现在需要持有子页面的指针呢？？？
    ConfigPage  *m_configPage   = nullptr;
    ChartPage   *m_chartPage    = nullptr;

    //---通信层---
    QThread      *m_commThread   = nullptr;//通信子线程comm是什么意思？？？
    ModbusClient *m_modbusClient = nullptr;//Modbus通信对象运行在子线程
};

#endif // MAINWINDOW_H













