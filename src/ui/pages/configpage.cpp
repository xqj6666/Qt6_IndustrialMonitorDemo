//===配置界面：配置持久化，读取和保存====
#include "configpage.h"
#include "core/logger.h"

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSettings>
//核心基类，QApplication的父类:获取exe文件所在目录常用
#include <QCoreApplication>
//目录操作类
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

ConfigPage::ConfigPage(QWidget *parent)
    :QWidget(parent)
{
    initUI();
    initConnect();
    loadConfig();
}

ConfigPage::~ConfigPage()
{

}

QString ConfigPage::ipAddress() const
{
    return m_ipEdit->text();
}

quint16 ConfigPage::port() const
{
    //对于小众类型进行强制类型转换，SpinBox存储的是整数
    return static_cast<quint16>(m_portSpin->value());
}

int ConfigPage::pollInterval() const
{
    return m_pollIntervalSpin->value();
}

void ConfigPage::initUI()
{
    //-----外层垂直布局-----
    auto *mainLayout = new QVBoxLayout(this);

    //-----分组框（界面的分组与隔离带标题）-----
    auto *groupBox = new QGroupBox("Modbus-TCP 设备连接配置", this);
    auto *gridLayout = new QGridLayout(groupBox);

    //-----第0行：IP地址-----
    gridLayout->addWidget(new QLabel("IP地址：",this),0,0);
    m_ipEdit = new QLineEdit("192.168.1.100",this);
    gridLayout->addWidget(m_ipEdit,0,1);

    //-----第1行：端口号-----
    gridLayout->addWidget(new QLabel("端口号：",this),1,0);
    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(1,65535);
    m_portSpin->setValue(502);
    gridLayout->addWidget(m_portSpin,1,1);

    //-----第2行：轮询周期-----
    gridLayout->addWidget(new QLabel("轮询周期：",this),2,0);
    m_pollIntervalSpin = new QSpinBox(this);
    m_pollIntervalSpin->setRange(100,10000);
    m_pollIntervalSpin->setSingleStep(100);
    m_pollIntervalSpin->setValue(1000);
    m_pollIntervalSpin->setSuffix("ms");
    gridLayout->addWidget(m_pollIntervalSpin,2,1);

    //-----第3行：按钮-----
    auto *btnLayout = new QHBoxLayout();
    m_resetBtn = new QPushButton("恢复默认",this);
    m_saveBtn  = new QPushButton("保存配置",this);
    btnLayout->addStretch();
    btnLayout->addWidget(m_resetBtn);
    btnLayout->addWidget(m_saveBtn);
    gridLayout->addLayout(btnLayout,3,0,1,2);//从第3行0列开始，占1行2列

    //把分组框加入主布局
    mainLayout->addWidget(groupBox);
    mainLayout->addStretch();//让内容上靠，下方留白
}


void ConfigPage::initConnect()
{
    //保存按钮点击信号 连接到 保存槽函数
    connect(m_saveBtn,&QPushButton::clicked,this,&ConfigPage::saveConfig);

    //恢复默认按钮点击信号 连接到 恢复默认槽函数
    connect(m_resetBtn,&QPushButton::clicked,this,&ConfigPage::resetConfig);
}

void ConfigPage::loadConfig()
{
    //配置文件路径：可执行文件同级目录/config/settings.ini
    //获取exe文件路径并进行拼接得到配置文件目录
    QString configPath = QCoreApplication::applicationDirPath()+"/config/settings.ini";
    //指定文件路径，使用ini文件格式
    QSettings settings(configPath,QSettings::IniFormat);

    //读取配置，如果不存在则使用默认配置
    //读取键中的内容，第二个参数表示默认值
    m_ipEdit->setText(settings.value("Connection/IP",DEFAULT_IP).toString());
    m_portSpin->setValue(settings.value("Connection/Port",DEFAULT_PORT).toInt());
    m_pollIntervalSpin->setValue(settings.value("Connection/PollInterval",DEFAULT_POLL_INTERVAL).toInt());

    // ---- 加载后校验，非法值用默认值替换 ----一打开程序就弹窗会破坏用户体验，所以出错就改成默认值
    QHostAddress addr;
    if (!addr.setAddress(m_ipEdit->text().trimmed())) {
        m_ipEdit->setText(DEFAULT_IP);
        Logger::warning("配置文件 IP 格式异常，已恢复默认值");
    }

    if (m_portSpin->value() == 0) {
        m_portSpin->setValue(DEFAULT_PORT);
        Logger::warning("配置文件端口异常，已恢复默认值");
    }

    qDebug()<<"配置已从"<<configPath<<"加载";
}

void ConfigPage::saveConfig()
{
    //提前检测目录的可写性，如果没有权限就提醒用户（待做）

    //校验数据是否合法
    QString error = validateInput();
    if(!error.isEmpty()){
        QMessageBox::warning(this, "参数错误", error);
        return;
    }

    //确保配置目录存在
    QString configDir = QCoreApplication::applicationDirPath() + "/config";
    QDir dir(configDir);
    //如果config文件夹不存在，就创建这个文件夹
    if(!dir.exists()){dir.mkpath(configDir);}

    QString configPath = configDir + "/settings.ini";
    QSettings settings(configPath,QSettings::IniFormat);//再次构造QSettings对象，指向ini文件

    //保存配置
    settings.setValue("Connection/IP",m_ipEdit->text());
    settings.setValue("Connection/Port",m_portSpin->value());
    settings.setValue("Connection/PollInterval",m_pollIntervalSpin->value());

    //同步到磁盘
    settings.sync();
    QString log = "配置已保存到"+configPath;
    Logger::info(log);

}

void ConfigPage::resetConfig()
{
    m_ipEdit->setText(DEFAULT_IP);
    m_portSpin->setValue(DEFAULT_PORT);
    m_pollIntervalSpin->setValue(DEFAULT_POLL_INTERVAL);
    Logger::info("重置配置");
}

QString ConfigPage::validateInput() const
{
    QString ip = m_ipEdit->text().trimmed();
    if(ip.isEmpty()){
        return "地址不能为空";
    }

    QHostAddress addr;
    if(!addr.setAddress(ip)){
        return QString("IP地址格式不正确：%1").arg(ip);
    }

    if(m_portSpin->value() == 0){
        return "端口号不能为0";
    }

    if(m_pollIntervalSpin->value()<100){
        return "轮询周期不能小于100ms";
    }
    return {};
}












