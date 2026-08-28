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

//编译器常量，日志信号发射时增强可读性
static constexpr int LOG_DEBUG   = 0;
static constexpr int LOG_INFO    = 1;
static constexpr int LOG_WARNING = 2;
static constexpr int LOG_ERROR   = 3;

class ModbusClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ModbusClient)

public:
    explicit ModbusClient(QObject *parent = nullptr);
    ~ModbusClient() override;

    //连接状态枚举
    enum class DeviceState{
        Disconnected,
        Connecting,
        Connected,
        Error
    };
    Q_ENUM(DeviceState)

public slots:
    //主线程不能直接安全的调用子线程的函数，所以要通过信号进行调用
    void init();//初始化，在子线程中执行
    void connectToDevice(const QString &ip, quint16 port);
    void disconnectDevice();
    void startPolling(int interalMs, int startAddr, int registerCount);
    void stopPolling();

signals:
    //---发给主线程的信号---
    void deviceStateChanged(ModbusClient::DeviceState state,const QString &ip,quint16 port);
    void registerDataReady(int startAddr,const QVector<quint16> &values);
    void errorOccurred(const QString &errorMsg);
    void logMessage(const QString &msg, int level);

private slots:
    //---内部槽,在子线程中执行---
    void onStateChanged(QModbusDevice::State state);
    void onDeviceError(QModbusDevice::Error error);
    void doPoll();

private:
    QModbusTcpClient *m_modbusClient = nullptr;//真正底层干活的对象：TCP spcket ModbusTCP报文
    QTimer           *m_pollTimer    = nullptr;

    QString m_ip;
    quint16 m_port          = 0;
    int     m_startAddr     = 0;
    int     m_registerCount = 10;
};

#endif // MODBUSCLIENT_H
























