//===设备监控页面：显示连接情况和寄存器数据===
#ifndef MONITORPAGE_H
#define MONITORPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QGroupBox;
class QTableWidget;
class QPushButton;
QT_END_NAMESPACE

//自定义指示灯控件，不在Qt命名空间
class IndicatorLight;

class MonitorPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MonitorPage)

public:
    explicit MonitorPage(QWidget* parent = nullptr);
    ~MonitorPage() override;

signals:
    //信号链路：monitorpage->mainwindow->跨线程modbusclient
    void connectRequested();
    void disconnectRequested();

public slots:
    //数据链路：modbusclient->mainwindow->monitorpage
    void onDeviceStateChanged(int state, const QString &ip, quint16 port);
    void onRegisterDataReady(int startAddr, const QVector<quint16> &values);

private:
    void initUI();
    void initConnect();

    //---设备状态面板---
    QGroupBox       *m_statusGroup = nullptr;
    IndicatorLight  *m_statusLight = nullptr;
    QLabel          *m_statusLabel = nullptr;
    QLabel          *m_ipLabel     = nullptr;
    QLabel          *m_portLabel   = nullptr;
    QPushButton     *m_connectBtn  = nullptr;
    QPushButton     *m_disconnectBtn = nullptr;

    //---寄存器数据表格---
    QGroupBox   *m_tableGroup    = nullptr;
    QTableWidget  *m_registerTable = nullptr;
};

#endif // MONITORPAGE_H
