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
    if(m_state != state){//加判断最主要的原因是代码意图清晰，只有状态真的改变才需要重绘
        m_state = state;
        update();//不是立即重绘，而是标记一个重绘请求Qt会在下一帧统一处理，如果短时间大量调用，会合并成一次
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
    Q_UNUSED(event);//我们没有用到event参数，消除警告

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//抗锯齿

    //根据状态选择颜色
    QColor color;
    switch(m_state){
    case LightState::Connected:     color = QColor(0,200,0);break;//绿色 已连接
    case LightState::Error:         color = QColor(220,0,0);break;//红色 异常
    case LightState::Connecting:    color = QColor(255,200,0);break;//黄色，连接中
    case LightState::Disconnected:  color = QColor(160,160,160);break;//灰色,未链接
    }

    //计算居中绘制的圆
    int x = (width() - m_diameter) / 2;//左上角模式，得出圆的矩形的左上角，兼容性好
    int y = (height() - m_diameter)/2;//当做仪表盘，坐标变换的时候圆心模式好，数据直觉完美

    painter.setPen(QPen(Qt::darkGray,1));//画笔，灰色，像素1
    painter.setBrush(color);
    painter.drawEllipse(x,y,m_diameter,m_diameter);
}
























