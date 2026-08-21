#ifndef INDICATORLIGHT_H
#define INDICATORLIGHT_H

#include <QWidget>

class IndicatorLight : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(IndicatorLight)

public:
    enum class LightState{
        Disconnected,//灰色-未链接
        Connected,//绿色-已连接
        Error,//红色-异常断开
        Connecting//黄色,正在连接/警告
    };

    explicit IndicatorLight(QWidget *parent = nullptr);
    ~IndicatorLight() override;

    void setState(LightState state);
    LightState state() const;

    void setDiameter(int deameter);//集合参数用直径，计算居中位置方便
    //QWidget最小尺寸提示，让布局管理器知道这个控件需要多大
    QSize sizeHint() const override;//告诉布局管理器理想尺寸
    QSize minimumSizeHint() const override;//告诉布局管理器最小尺寸

protected:
    void paintEvent(QPaintEvent *event) override;//protected和基类保持一致

private:
    LightState m_state = LightState::Disconnected;
    int m_diameter = 20;
};

#endif // INDICATORLIGHT_H
























