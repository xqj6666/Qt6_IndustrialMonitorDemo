//===图表界面，把数据绘制成图表===
#ifndef CHARTPAGE_H
#define CHARTPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QSplitter;
QT_END_NAMESPACE

class QCustomPlot;

class ChartPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ChartPage)

public:
    explicit ChartPage(QWidget* parent = nullptr);
    ~ChartPage() override;

public slots:
    //mainwindow->chartpage 接收寄存器数据
    void onRegisterDataReady(int startAddr, const QVector<quint16> &values);

private:
    void initUI();
    void initConnect();
    void setupPlot(QCustomPlot *plot, const QString &title, const QString &yLabel);

    QSplitter   *m_splitter     = nullptr;
    QCustomPlot *m_tempPlot      = nullptr;//温度曲线
    QCustomPlot *m_pressPlot    = nullptr;//压力曲线

    static constexpr double TIME_WINDOW = 60.0;//显示最近60s

    int m_lastTempGraphIndex = 0;//上一次温度数据属于哪条曲线
    int m_lastPressGraphIndex = 0;//上一次压力数据属于哪条曲线
};

#endif // CHARTPAGE_H
