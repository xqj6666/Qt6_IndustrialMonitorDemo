#include "modbusclient.h"

#include <QTimer>
#include <QModbusTcpClient>
#include <QModbusReply>

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
    emit logMessage(QString("正在连接设备 %1:%2 ...").arg(ip).arg(port),1);
}



















