#include "monitorpage.h"
#include "communication/modbusclient.h"
#include "ui/widgets/indicatorlight.h"

#include <QLabel>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

MonitorPage::MonitorPage(QWidget *parent)
    :QWidget(parent)
{
    initUI();
    initConnect();
}

MonitorPage::~MonitorPage()
{

}

void MonitorPage::initUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    //---第一部分：设备状态面板---
    m_statusGroup = new QGroupBox("设备状态",this);
    auto *statusLayout = new QHBoxLayout(m_statusGroup);

    //指示灯
    m_statusLight = new IndicatorLight(m_statusGroup);
    m_statusLight->setDiameter(24);
    m_statusLight->setState(IndicatorLight::LightState::Disconnected);

    //状态文字
    m_statusLabel = new QLabel("未连接",m_statusGroup);
    m_statusLabel->setMinimumWidth(80);

    //IP地址显示
    m_ipLabel = new QLabel("IP：--",m_statusGroup);
    m_ipLabel->setMinimumWidth(180);

    //端口显示
    m_portLabel = new QLabel("端口：--",m_statusGroup);
    m_portLabel->setMinimumWidth(100);


    //连接/断开按钮
    m_connectBtn = new QPushButton("连接设备",m_statusGroup);
    m_disconnectBtn = new QPushButton("断开连接",m_statusGroup);
    m_disconnectBtn->setEnabled(false);

    statusLayout->addWidget(m_statusLight);
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addSpacing(20);
    statusLayout->addWidget(m_ipLabel);
    statusLayout->addSpacing(20);
    statusLayout->addWidget(m_portLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_connectBtn);
    statusLayout->addWidget(m_disconnectBtn);

    mainLayout->addWidget(m_statusGroup);

    //---第2部分：寄存器数据表格---
    m_tableGroup = new QGroupBox("寄存器数据",this);
    auto *tableLayout = new QVBoxLayout(m_tableGroup);

    m_registerTable = new QTableWidget(m_tableGroup);
    m_registerTable->setColumnCount(4);
    m_registerTable->setHorizontalHeaderLabels({"序号","地址","名称","值"});

    //表头设置
    m_registerTable->horizontalHeader()->setStretchLastSection(true);//最后一列自动填充
    m_registerTable->verticalHeader()->setVisible(false);//隐藏左侧行号
    m_registerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止用户编辑
    m_registerTable->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中

    //设置10行占位数据
    m_registerTable->setRowCount(10);
    for (int i = 0; i < 10; ++i) {
        m_registerTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        m_registerTable->setItem(i, 1, new QTableWidgetItem(QString("0x%1").arg(i, 4, 16, QChar('0'))));
        m_registerTable->setItem(i, 2, new QTableWidgetItem(QString("寄存器_%1").arg(i)));
        m_registerTable->setItem(i, 3, new QTableWidgetItem("--"));
    }

    tableLayout->addWidget(m_registerTable);
    mainLayout->addWidget(m_tableGroup);

    //上下比例 1:4，状态面板小，表格大
    mainLayout->setStretch(0,1);
    mainLayout->setStretch(1,4);
}

void MonitorPage::initConnect()
{
    //连接按钮->发信号给MainWindow
    connect(m_connectBtn, &QPushButton::clicked, this, &MonitorPage::connectRequested);
    //断开按钮
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MonitorPage::disconnectRequested);
}

void MonitorPage::onDeviceStateChanged(int state, const QString &ip, quint16 port)
{
    switch(static_cast<ModbusClient::DeviceState>(state))
    {
    case ModbusClient::DeviceState::Disconnected:
        m_statusLight->setState(IndicatorLight::LightState::Disconnected);
        m_statusLabel->setText("未连接");
        m_ipLabel->setText("IP:--");
        m_portLabel->setText("端口：--");
        m_connectBtn->setEnabled(true);
        m_disconnectBtn->setEnabled(false);
        break;

    case ModbusClient::DeviceState::Connecting:
        m_statusLight->setState(IndicatorLight::LightState::Connecting);
        m_statusLabel->setText("连接中");
        m_ipLabel->setText("IP：" + ip);
        m_portLabel->setText("端口：" + QString::number(port));
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(false);
        break;

    case ModbusClient::DeviceState::Connected:
        m_statusLight->setState(IndicatorLight::LightState::Connected);
        m_statusLabel->setText("已连接");
        m_ipLabel->setText("IP：" + ip);
        m_portLabel->setText("端口：" + QString::number(port));
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
        break;

    case ModbusClient::DeviceState::Error:
        m_statusLight->setState(IndicatorLight::LightState::Error);
        m_statusLabel->setText("连接异常");
        m_connectBtn->setEnabled(true);
        m_disconnectBtn->setEnabled(false);
        break;
    }
}

void MonitorPage::onRegisterDataReady(int startAddr,const QVector<quint16> &values)
{
    //把寄存器数据更新到表格的值列
    for(int i = 0; i<values.size(); ++i)
    {
        int row = startAddr + i;
        if(row < m_registerTable->rowCount())
        {
            m_registerTable->item(row,3)->setText(QString::number(values.at(i)));
        }
    }
}





















