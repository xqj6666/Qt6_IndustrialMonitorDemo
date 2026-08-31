//===主窗口界面===
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QTabWidget;
//分割器，需要setChildrenCollapsidle(false),防止操作人员把面板拖消失，找不到
class QSplitter;
class QTextEdit;
class QThread;
QT_END_NAMESPACE

class Logger;
class ModbusClient;
class MonitorPage;
class ConfigPage;
class ChartPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindow)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    //mainwindow->modbusclient
    void requestConnect(const QString &ip, quint16 port);
    void requestDisconnect();
    void requestStartPolling(int intervalMs, int startAddr, int registerCount);
    void requestStopPolling();

private:
    //初始化
    void initUI();
    void initConnect();
    void initData();
    void initCommunication();

private slots:
    //数据链路：调用全局单例Logger->mainwindow
    void onLogMessage(const QString &formattedMsg,int level);
    //数据链路：modbusclient -> mainwindow
    void onDeviceStateChanged(int state, const QString &ip, quint16 port);

    //monitorpage->mainwindow->modbusclient
    void onConnectRequested();
    void onDisconnectRequested();
    //用户点击monitorpage的断开连接按钮,既然monitorpage也在主线程，为什么不直接发信号给modbusclient?
    //1.解耦，2.消息广播，多页面分发3.统一处理、过滤、日志4.页面生命周期风险：monitorpage有可能直接被销毁。动态new和delete

private:
    QSplitter   *m_mainSplitter = nullptr;//主分割器(上下分割)
    QTabWidget  *m_tabWidget    = nullptr;//多页面容器
    QTextEdit   *m_logView      = nullptr;//日志显示区域
    QLabel      *m_statusLabel  = nullptr;//连接信息标签

    //---页面指针---
    MonitorPage *m_monitorPage  = nullptr;
    ConfigPage  *m_configPage   = nullptr;
    ChartPage   *m_chartPage    = nullptr;

    //---通信层---
    QThread      *m_commThread   = nullptr;//comm==communication
    ModbusClient *m_modbusClient = nullptr;
};

#endif // MAINWINDOW_H













