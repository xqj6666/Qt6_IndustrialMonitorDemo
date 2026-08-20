#include "configpage.h"

#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>

ConfigPage::ConfigPage(QWidget *parent)
    :QWidget(parent)
{
    initUI();
    initConnect();
}

ConfigPage::~ConfigPage()
{

}

void ConfigPage::initUI()
{
    //-----外层垂直布局-----
    auto *mainLayout = new QVBoxLayout(this);

    //-----分组框-----
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
}












