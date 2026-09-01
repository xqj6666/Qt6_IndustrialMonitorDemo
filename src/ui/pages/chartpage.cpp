//===图表界面，把数据绘制成图表===
#include "chartpage.h"
#include "qcustomplot.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QDateTime>

ChartPage::ChartPage(QWidget *parent)
    : QWidget(parent)
{
    initUI();
    initConnect();
}

ChartPage::~ChartPage()
{
}

void ChartPage::initUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 上下分割两个图表
    m_splitter = new QSplitter(Qt::Vertical, this);

    // 温度曲线
    m_tempPlot = new QCustomPlot(m_splitter);
    setupPlot(m_tempPlot, "温度实时曲线", "温度 (\u00B0C)");
    m_splitter->addWidget(m_tempPlot);

    // 压力曲线
    m_pressPlot = new QCustomPlot(m_splitter);
    setupPlot(m_pressPlot, "压力实时曲线", "压力 (kPa)");
    m_splitter->addWidget(m_pressPlot);

    mainLayout->addWidget(m_splitter);
}

void ChartPage::initConnect()
{
}

// 通用曲线初始化
void ChartPage::setupPlot(QCustomPlot *plot, const QString &title, const QString &yLabel)
{
    // ---- 标题 ----
    plot->plotLayout()->insertRow(0);
    auto *titleElement = new QCPTextElement(plot, title, QFont("Microsoft YaHei", 12, QFont::Bold));
    titleElement->setTextColor(QColor(220, 220, 220));
    plot->plotLayout()->addElement(0, 0, titleElement);


    // ---- 深色背景 ----
    plot->setBackground(QColor(30, 30, 30));
    plot->axisRect()->setBackground(QColor(30, 30, 30));

    // ---- 坐标轴样式 ----
    plot->xAxis->setLabel("时间");
    plot->yAxis->setLabel(yLabel);

    // 坐标轴文字颜色
    plot->xAxis->setTickLabelColor(QColor(200, 200, 200));
    plot->yAxis->setTickLabelColor(QColor(200, 200, 200));
    plot->xAxis->setLabelColor(QColor(200, 200, 200));
    plot->yAxis->setLabelColor(QColor(200, 200, 200));

    // 坐标轴线条颜色
    plot->xAxis->setBasePen(QPen(QColor(100, 100, 100)));
    plot->yAxis->setBasePen(QPen(QColor(100, 100, 100)));
    plot->xAxis->setTickPen(QPen(QColor(100, 100, 100)));
    plot->yAxis->setTickPen(QPen(QColor(100, 100, 100)));
    plot->xAxis->setSubTickPen(QPen(QColor(80, 80, 80)));
    plot->yAxis->setSubTickPen(QPen(QColor(80, 80, 80)));

    // 网格线
    plot->xAxis->grid()->setPen(QPen(QColor(60, 60, 60)));
    plot->yAxis->grid()->setPen(QPen(QColor(60, 60, 60)));

    // X 轴用时间格式显示
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime());
    dateTicker->setDateTimeFormat("HH:mm:ss");
    plot->xAxis->setTicker(dateTicker);

    // ---- 分段着色：三条曲线对应三种颜色 ----

    // graph(0) 正常段 - 绿色
    plot->addGraph();
    plot->graph(0)->setPen(QPen(QColor(0, 200, 0), 2));
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    // graph(1) 警告段 - 黄色
    plot->addGraph();
    plot->graph(1)->setPen(QPen(QColor(255, 200, 0), 2));
    plot->graph(1)->setLineStyle(QCPGraph::lsLine);
    plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    // graph(2) 危险段 - 红色
    plot->addGraph();
    plot->graph(2)->setPen(QPen(QColor(255, 50, 50), 2));
    plot->graph(2)->setLineStyle(QCPGraph::lsLine);
    plot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 3));

    // 初始范围
    double now = QDateTime::currentDateTime().toSecsSinceEpoch();
    plot->xAxis->setRange(now - TIME_WINDOW, now);
    plot->yAxis->setRange(0, 1000);

    // 允许用户鼠标拖拽和缩放（仅水平方向）
    plot->setInteraction(QCP::iRangeDrag, true);
    plot->setInteraction(QCP::iRangeZoom, true);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);
    plot->axisRect()->setRangeZoom(Qt::Horizontal);
}

// 接收寄存器数据，更新曲线：分段着色 + 交接点重叠
void ChartPage::onRegisterDataReady(int startAddr, const QVector<quint16> &values)
{
    if (values.size() < 2)
        return;
    if (startAddr != 0)
        return;

    double now = QDateTime::currentDateTime().toSecsSinceEpoch();

    // ============================================================
    // 温度曲线（阈值：50 黄色，70 红色）
    // ============================================================
    int tempGraphIndex;
    if (values.at(0) >= 70) {
        tempGraphIndex = 2;  // 红色
    } else if (values.at(0) >= 50) {
        tempGraphIndex = 1;  // 黄色
    } else {
        tempGraphIndex = 0;  // 绿色
    }

    // 颜色切换时，把当前点同时添加到新旧两条曲线，避免断开
    if (tempGraphIndex != m_lastTempGraphIndex) {
        m_tempPlot->graph(m_lastTempGraphIndex)->addData(now, values.at(0));
    }
    m_tempPlot->graph(tempGraphIndex)->addData(now, values.at(0));
    m_lastTempGraphIndex = tempGraphIndex;

    // 三条曲线都要移除旧数据
    for (int i = 0; i < 3; ++i) {
        m_tempPlot->graph(i)->data()->removeBefore(now - TIME_WINDOW);
    }
    m_tempPlot->xAxis->setRange(now - TIME_WINDOW, now);
    m_tempPlot->yAxis->rescale(true);
    m_tempPlot->replot();

    // ============================================================
    // 压力曲线（阈值：500 黄色，700 红色）
    // ============================================================
    int pressGraphIndex;
    if (values.at(1) >= 700) {
        pressGraphIndex = 2;  // 红色
    } else if (values.at(1) >= 500) {
        pressGraphIndex = 1;  // 黄色
    } else {
        pressGraphIndex = 0;  // 绿色
    }

    if (pressGraphIndex != m_lastPressGraphIndex) {
        m_pressPlot->graph(m_lastPressGraphIndex)->addData(now, values.at(1));
    }
    m_pressPlot->graph(pressGraphIndex)->addData(now, values.at(1));
    m_lastPressGraphIndex = pressGraphIndex;

    for (int i = 0; i < 3; ++i) {
        m_pressPlot->graph(i)->data()->removeBefore(now - TIME_WINDOW);
    }
    m_pressPlot->xAxis->setRange(now - TIME_WINDOW, now);
    m_pressPlot->yAxis->rescale(true);
    m_pressPlot->replot();
}
















