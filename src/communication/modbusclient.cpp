#include "modbusclient.h"

#include <QTimer>
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QVariant>//通用容器

ModbusClient::ModbusClient(QObject *parent)
    :QObject(parent)
{

}

//===将mainwindow中的ModbusClient对象移入子线程之后，再创建ModbusClient的子对象，保证子对象的线程亲和性
void ModbusClient::init()
{
    //这个函数在子线程里执行，通过connect QThred::started触发
    //此时对象已经在子线程中，创建的子对象线程亲和性也是子线程
    m_modbusClient = new QModbusTcpClient();

    //轮询计时器
    m_pollTimer = new QTimer();
    m_pollTimer->setSingleShot(false);

    //重连计时器
    m_reconnectTimer = new QTimer();
    m_reconnectTimer->setSingleShot(false);
    m_reconnectTimer->setInterval(RECONNECT_INTERVAL_MS);

    //---ModbusTcpClient内部信号连接ModbusClient的三个内部槽---
    connect(m_modbusClient,&QModbusTcpClient::stateChanged,this,&ModbusClient::onStateChanged);
    connect(m_modbusClient,&QModbusTcpClient::errorOccurred,this,&ModbusClient::onDeviceError);
    connect(m_pollTimer,&QTimer::timeout,this,&ModbusClient::doPoll);
    //重连定时器超时 → 执行一次重连
    connect(m_reconnectTimer,&QTimer::timeout,this,&ModbusClient::doReconnect);
}

ModbusClient::~ModbusClient()
{
    //停止轮询器
    if(m_pollTimer){
        m_pollTimer->stop();
    }
    //停止重连计时器
    if(m_reconnectTimer){
        m_reconnectTimer->stop();
    }

    //断开Modbus设备TCP连接
    if(m_modbusClient){
        if(m_modbusClient->state() != QModbusDevice::UnconnectedState){
            m_modbusClient->disconnectDevice();
        }
        m_modbusClient->deleteLater();
    }
    //已经停止，可以直接delete
    if(m_pollTimer){
        delete m_pollTimer;
        m_pollTimer = nullptr;
    }
    if(m_reconnectTimer){
        delete m_reconnectTimer;
        m_reconnectTimer = nullptr;
    }
}

void ModbusClient::connectToDevice(const QString &ip,quint16 port)
{
    m_ip = ip;
    m_port = port;

    //连接新设备时，重置重连状态
    m_reconnectAttempts = 0;
    m_userDisconnected = false;
    if(m_reconnectTimer){
        m_reconnectTimer->stop();
    }

    //如果当前已连接，先断开再重新连接
    if(m_modbusClient->state() != QModbusDevice::UnconnectedState){
        m_modbusClient->disconnectDevice();
    }

    //设置连接参数
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter,port);

    //设置超时时间3000ms,常规合理时间
    m_modbusClient->setTimeout(3000);
    //重试次数3次，最多四次，初始1次+重试3次
    m_modbusClient->setNumberOfRetries(3);
    //发起异步连接，不阻塞。连接成功后触发stateChanged信号->onStateChanged槽
    //内部封装了Socket和TCP握手等代码
    m_modbusClient->connectDevice();

    emit deviceStateChanged(DeviceState::Connecting,m_ip,m_port);
    //通知UI正在建立连接
    emit logMessage(QString("正在连接设备 %1:%2 ...").arg(ip).arg(port),LOG_INFO);
}

void ModbusClient::disconnectDevice()
{
    //先停止轮询，否则在断开的连接上发请求，行为不可预测
    stopPolling();

    //设置标志
    m_userDisconnected = true;
    //停止重连计时器
    if(m_reconnectTimer){
        m_reconnectTimer->stop();
    }
    //重置重连计数
    m_reconnectAttempts = 0;

    //断开Modbus设备
    if(m_modbusClient->state() != QModbusDevice::UnconnectedState){
        m_modbusClient->disconnectDevice();
    }

    emit logMessage("主动断开设备连接",LOG_INFO);
}

void ModbusClient::startPolling(int intervalMs, int startAddr, int registerCount)
{
    //保存轮询参数，doPoll()执行时会读取这些值
    m_startAddr = startAddr;
    m_registerCount = registerCount;

    m_pollTimer->setInterval(intervalMs);
    m_pollTimer->start();

    emit logMessage(QString("启动轮询：间隔=%1ms,起始地址=0x%2,数量=%3")
                        .arg(intervalMs)
                        .arg(startAddr,4,16,QChar('0'))
                        .arg(registerCount),
                    LOG_INFO);
}

void ModbusClient::stopPolling()
{
    if(m_pollTimer->isActive()){
        m_pollTimer->stop();
        emit logMessage("停止轮询",LOG_INFO);
    }
}

//当连接状态变化时，自动调用
void ModbusClient::onStateChanged(QModbusDevice::State state)
{
    switch(state)
    {
    case QModbusDevice::UnconnectedState:
        emit deviceStateChanged(DeviceState::Disconnected,m_ip,m_port);
        emit logMessage("设备已断开",LOG_WARNING);
        //不是用户主动断开、重连次数没耗尽、定时器没在跑，才启动重连
        if(!m_userDisconnected
            && m_reconnectAttempts < MAX_RECONNECT_ATTEMPTS
            && !m_reconnectTimer->isActive())
        {
            m_reconnectAttempts = 0;
            m_reconnectTimer->start();
            emit logMessage("启动断线重连...",LOG_INFO);
        }
        break;

    case QModbusDevice::ConnectingState:
        emit deviceStateChanged(DeviceState::Connecting,m_ip,m_port);
        break;

    case QModbusDevice::ConnectedState:
        emit deviceStateChanged(DeviceState::Connected,m_ip,m_port);
        emit logMessage(QString("设备连接成功: %1:%2").arg(m_ip).arg(m_port),LOG_INFO);
        //连接成功，停止重连定时器，重置计数器
        m_reconnectTimer->stop();
        m_reconnectAttempts = 0;
        break;

    case QModbusDevice::ClosingState:
        break;
    }
}

//错误处理，内部槽
void ModbusClient::onDeviceError(QModbusDevice::Error error)
{
    if(error == QModbusDevice::NoError)
        return;

    //获取错误描述文本
    QString errorMsg = m_modbusClient->errorString();
    emit errorOccurred(errorMsg);
    emit logMessage("Modbus错误：" + errorMsg, LOG_ERROR);

    //如果是连接错误，向主线程发送错误信号
    if(error == QModbusDevice::ConnectionError)
    {
        emit deviceStateChanged(DeviceState::Error,m_ip,m_port);
    }
}

//执行一次轮询,通信层最核心的函数
void ModbusClient::doPoll()
{
    //前置检查:如果设备未连接，不发请求
    if(m_modbusClient->state() != QModbusDevice::ConnectedState)
    {
        return;
    }

    //构造请求
    //参数一：寄存器类型(HoldingRegisters = 保持寄存器，功能码0x03)
    //参数二：起始地址
    //参数三：读取数量
    QModbusDataUnit request(QModbusDataUnit::HoldingRegisters,m_startAddr,m_registerCount);

    //发送请求
    //参数一：数据单元
    //参数二：服务器地址（Modbus-TCP 一般为1）
    //返回值：QModbusReply*，请求的异步句柄
    QModbusReply *reply = m_modbusClient->sendReadRequest(request,1);

    if(!reply)
    {
        //发送失败，可能连接已断开或者内部错误
        emit logMessage("发送请求失败",LOG_ERROR);
        return;
    }

    //异步等待响应：当设备返回数据时，reply的finished信号会触发
    connect(reply, &QModbusReply::finished, this, [this, reply]()
            {
                //检查reply是否有错误
                if(reply->error() != QModbusDevice::NoError)
                {
                    emit logMessage("读取失败：" + reply->errorString(),LOG_ERROR);
                    reply->deleteLater();//用完必须deleteLater,否则内存泄露
                    return;
                }

                //解析返回数据，result()返回QModbusDataUnit，包含寄存器数据
                QModbusDataUnit unit = reply->result();

                //把寄存器值存入Qvector
                QVector<quint16> values;
                values.reserve(unit.valueCount());//预分配内存，避免多次扩容
                for(int i = 0;i < unit.valueCount(); ++i)
                {
                    values.append(unit.value(i));
                }

                //发信号给主线程，携带寄存器地址和数据
                emit registerDataReady(unit.startAddress(),values);

                //用完reply，延迟销毁
                reply->deleteLater();
            });
}

//重连定时器超时执行一次重连
void ModbusClient::doReconnect()
{
    //如果当前不是断开状态，跳过本次，等下次定时器触发
    //不增加计数，避免白涨
    if(m_modbusClient->state() != QModbusDevice::UnconnectedState) {
        return;
    }

    //真正尝试连接时才计数
    m_reconnectAttempts++;

    //超过最大重连次数，停止重连
    if(m_reconnectAttempts > MAX_RECONNECT_ATTEMPTS)
    {
        m_reconnectTimer->stop();
        //注意：不重置 m_reconnectAttempts，保持在 MAX + 1
        //这样 onStateChanged 里 m_reconnectAttempts < MAX_RECONNECT_ATTEMPTS 不满足，不会重新启动重连
        emit logMessage(QString("重连 %1 次均失败，设备故障，请手动连接或检查设备").arg(MAX_RECONNECT_ATTEMPTS), LOG_ERROR);
        emit reconnectedFaild(m_ip, m_port);
        emit deviceStateChanged(DeviceState::Error, m_ip, m_port);
        return;
    }

    //尝试连接
    m_modbusClient->connectDevice();
    emit deviceStateChanged(DeviceState::Connecting, m_ip, m_port);
    emit logMessage(QString("第 %1/%2 次重连尝试...").arg(m_reconnectAttempts).arg(MAX_RECONNECT_ATTEMPTS), LOG_INFO);
}