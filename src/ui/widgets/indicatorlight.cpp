#include "indicatorlight.h"

#include <QPainter>

IndicatorLight::IndicatorLight(QWidget *parent)
    :QWidget(parent)
{

}

IndicatorLight::~IndicatorLight()
{

}

void IndicatorLight::setState(LightState state)
{
    //语义清晰，只有状态改变才需要重绘
    if(m_state != state){
        m_state = state;
        //不是立即重绘，而是标记一个重绘请求Qt会在下一帧统一处理，如果短时间大量调用，会合并成一次
        update();
    }
}

IndicatorLight::LightState IndicatorLight::state() const
{
    return m_state;
}

void IndicatorLight::setDiameter(int diameter)
{
    m_diameter = diameter;
    update();
}

QSize IndicatorLight::sizeHint() const
{
    return QSize(m_diameter + 4, m_diameter + 4);
}

QSize IndicatorLight::minimumSizeHint() const
{
    return QSize(16,16);
}

void IndicatorLight::paintEvent(QPaintEvent *event)
{
    //我们没有用到event参数，消除警告
    Q_UNUSED(event);

    QPainter painter(this);
    //抗锯齿
    painter.setRenderHint(QPainter::Antialiasing);

    //根据状态选择颜色
    QColor color;
    switch(m_state){
    case LightState::Connected:     color = QColor(0,200,0);break;//绿色 已连接
    case LightState::Error:         color = QColor(220,0,0);break;//红色 异常
    case LightState::Connecting:    color = QColor(255,200,0);break;//黄色，连接中
    case LightState::Disconnected:  color = QColor(160,160,160);break;//灰色,未链接
    }

    //计算居中绘制的圆
    //目前是左上角模式，得出圆的矩形的左上角，兼容性好
    //如果当做仪表盘，坐标变换的时候圆心模式好，数据直觉完美
    int x = (width() - m_diameter) / 2;
    int y = (height() - m_diameter)/2;

    //画笔，灰色，像素1
    painter.setPen(QPen(Qt::darkGray,1));
    painter.setBrush(color);
    painter.drawEllipse(x,y,m_diameter,m_diameter);
}
























