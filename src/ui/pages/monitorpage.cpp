#include "monitorpage.h"

#include <QLabel>
#include <QVBoxLayout>

MonitorPage::MonitorPage(QWidget* parent)
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
    m_placeholderLabel = new QLabel("设备监控页-待填充",this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_placeholderLabel);
}

void MonitorPage::initConnect()
{
    //后续添加信号槽
}