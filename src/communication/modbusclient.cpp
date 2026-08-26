#include "modbusclient.h"

#include <QTimer>
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QVariant>//通用容器

ModbusClient::ModbusClient(QObject *parent)
    :QObject(parent)
{
    //使用构造函数的时候不要指定parent
    //有parent的对象无法moveToThread
    m_modbusClient = new QModbusTcpClient();

    //创建轮询定时器
    //同样不指定parent,定时器会在子线程工作
    m_pollTimer = new QTimer();
    m_pollTimer->setSingleShot(false);//周期触发

    //---连接ModbusTcpClient内部信号---？？？头文件中好像没有定义
    //设备状态变化信号
    connect(m_modbusClient,&QModbusTcpClient::stateChanged,this,&ModbusClient::onStateChanged);
    //设备内部错误信号
    connect(m_modbusClient,&QModbusTcpClient::errorOccurred,this,&ModbusClient::onDeviceError);
    //连接轮询定时器
    connect(m_pollTimer,&QTimer::timeout,this,&ModbusClient::doPoll);
}

ModbusClient::~ModbusClient()
{
    //停止轮询器
    if(m_pollTimer){
        m_pollTimer->stop();
    }

    //断开Modbus 设备连接
    if(m_modbusClient){
        if(m_modbusClient->state() != QModbusDevice::UnconnectedState){
            m_modbusClient->disconnectDevice();
        }
        m_modbusClient->deleteLater();//用deleteLater避免子线程析构问题
    }

    if(m_pollTimer){
        delete m_pollTimer;
        m_pollTimer = nullptr;
    }
}

//连接设备ip和端口，Modbus-TCP默认端口502
void ModbusClient::connectToDevice(const QString &ip,quint16 port)//为什么端口号要用quint16？？
{
    //保存连接参数，后面断线重连用
    m_ip = ip;
    m_port = port;

    //如果当前已连接，先断开再重新连接
    if(m_modbusClient->state() != QModbusDevice::UnconnectedState){
        m_modbusClient->disconnectDevice();
    }

    //设置连接参数
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);//QVariant类型???
    m_modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter,port);

    //设置超时时间3000s
    m_modbusClient->setTimeout(3000);
    //重试次数3次
    m_modbusClient->setNumberOfRetries(3);
    //发起异步连接，不阻塞。连接成功后触发stateChanged型号->onStateChanged槽
    m_modbusClient->connectDevice();
    //通知UI:正在连接中
    emit deviceStateChanged(DeviceState::Connecting,m_ip,m_port);
    emit logMessage(QString("正在连接设备 %1:%2 ...").arg(ip).arg(port),1);// 1=Info
}

//断开连接
void ModbusClient::disconnectDevice()
{
    //先停止轮询
    stopPolling();
    //断开Modbus设备
    if(m_modbusClient->state() != QModbusDevice::UnconnectedState){
        m_modbusClient->disconnectDevice();
    }

    emit logMessage("主动断开设备连接",1);//1=Info
}

//启动轮询(毫秒，起始寄存器地址，读取寄存器数量)
void ModbusClient::startPolling(int intervalMs, int startAddr, int registerCount)
{
    //保存轮询参数
    m_startAddr = startAddr;
    m_registerCount = registerCount;

    //设置定时器间隔
    m_pollTimer->setInterval(intervalMs);

    //启动定时器
    m_pollTimer->start();

    emit logMessage(QString("启动轮询：间隔=%1ms,起始地址=0x%2,数量=%3")
                        .arg(intervalMs)
                        .arg(startAddr,4,16,QChar('0'))
                        .arg(registerCount),
                    1);//1=Info为什么不直接写Info？？？
}

//停止轮询
void ModbusClient::stopPolling()
{
    if(m_pollTimer->isActive()){
        m_pollTimer->stop();
        emit logMessage("停止轮询",1);//1=Info  为什么没有引用logger也能发logMessage信号？？？
    }
}

//当连接状态变化时，自动调用，内部槽是什么意思???
void ModbusClient::onStateChanged(QModbusDevice::State state)
{
    switch(state)
    {
    case QModbusDevice::UnconnectedState://设备断开
        emit deviceStateChanged(DeviceState::Disconnected,m_ip,m_port);
        emit logMessage("设备已断开",2);//2=Warning  什么情况下枚举可以直接隐式转换成数字????
        break;

    case QModbusDevice::ConnectingState://正在连接中
        emit deviceStateChanged(DeviceState::Connecting,m_ip,m_port);
        break;

    case QModbusDevice::ConnectedState://连接成功
        emit deviceStateChanged(DeviceState::Connected,m_ip,m_port);
        emit logMessage(QString("设备连接成功 %1%2").arg(m_ip).arg(m_port),1);//1=Info
        break;

    case QModbusDevice::ClosingState://正在关闭连接
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
    emit logMessage("Modbus错误：" + errorMsg, 3);//3=Error

    //如果是连接错误，通知UI状态变成Error
    if(error == QModbusDevice::ConnectionError)
    {
        emit deviceStateChanged(DeviceState::Error,m_ip,m_port);
    }
}

//执行一次轮询,通信层最核心的函数
void ModbusClient::doPoll()
{
    //前置检查:如果设备未链接，不发请求
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
        //发送失败，可能链接已断开或者内部错误
        emit logMessage("发送请求失败",3);//3=Error
        return;
    }

    //异步等待响应：当设备返回数据时，reply的finished信号会触发
    connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        //检查reply是否有错误
        if(reply->error() != QModbusDevice::NoError)
        {
            emit logMessage("读取失败：" + reply->errorString(),3);//3=Error
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

        //发信号给主线程，鞋带寄存器地址和数据
        emit registerDataReady(unit.startAddress(),values);

        //用完reply，延迟销毁
        reply->deleteLater();
    }
            );
}

















