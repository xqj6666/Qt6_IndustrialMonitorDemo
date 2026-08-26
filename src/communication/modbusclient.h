#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>
#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

class ModbusClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ModbusClient)

public:
    explicit ModbusClient(QObject *parent = nullptr);
    ~ModbusClient() override;

    //连接状态枚举
    enum class DeviceState{
        Disconnected,//未连接
        Connecting,//连接中
        Connected,//已连接
        Error//异常
    };
    Q_ENUM(DeviceState)

public slots:
    //---由主线程通过信号好触发，在子线程中执行---

    //连接设备
    void connectToDevice(const QString &ip, quint16 port);
    //断开连接
    void disconnectDevice();
    //启动轮询读寄存器
    void startPolling(int interalMs, int startAddr, int registerCount);
    //停止轮询
    void stopPolling();

signals:
    //---发给主线程的信号---
    void deviceStateChanged(ModbusClient::DeviceState state,const QString &ip,quint16 port);
    //寄存器数据返回
    void registerDataReady(int startAddr,const QVector<quint16> &values);
    //错误信息
    void errorOccurred(const QString &errorMsg);
    //日志信号(转发给Logger)
    void logMessage(const QString &msg, int level);

private slots:
    //---内部槽,在子线程中执行---
    void onStateChanged(QModbusDevice::State state);
    void onDeviceError(QModbusDevice::Error error);
    void doPoll();//执行一次轮询

private:
    QModbusTcpClient *m_modbusClient = nullptr;//子对象QModbusTcpClient
    QTimer           *m_pollTimer    = nullptr;//轮询计时器

    QString m_ip;
    quint16 m_port          = 0;
    int     m_startAddr     = 0;
    int     m_registerCount = 10;
};

#endif // MODBUSCLIENT_H
























