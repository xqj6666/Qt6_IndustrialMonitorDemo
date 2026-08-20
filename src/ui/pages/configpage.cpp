#include "configpage.h"

#include <QLabel>
#include <QVBoxLayout>

ConfigPage::ConfigPage(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnect();
}

ConfigPage::~ConfigPage()
{
}

void ConfigPage::initUI()
{
    m_placeholderLabel = new QLabel("参数配置页 - 待填充", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_placeholderLabel);
}

void ConfigPage::initConnect()
{
}