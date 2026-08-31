//===图表界面，把数据绘制成图表===
#include "chartpage.h"

#include <QLabel>
#include <QVBoxLayout>

ChartPage::ChartPage(QWidget* parent)
    :QWidget(parent)
{
    initUI();
    initConnect();
}

ChartPage::~ChartPage()
{

}

void ChartPage::initUI()
{
    m_placeholderLabel = new QLabel("数据曲线页-待填充",this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_placeholderLabel);
}

void ChartPage::initConnect()
{

}