#ifndef MONITORPAGE_H
#define MONITORPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QGroupBox;
class QTableWidget;
QT_END_NAMESPACE

class IndicatorLight;//自定义控件，不在Qt命名空间

class MonitorPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MonitorPage)

public:
    explicit MonitorPage(QWidget* parent = nullptr);
    ~MonitorPage() override;

private:
    void initUI();
    void initConnect();

    //---设备状态面板---
    QGroupBox       *m_statusGroup = nullptr;
    IndicatorLight  *m_statusLight = nullptr;
    QLabel          *m_statusLabel = nullptr;
    QLabel          *m_ipLabel     = nullptr;
    QLabel          *m_portLabel   = nullptr;

    //---寄存器数据表格---
    QGroupBox   *m_tableGroup    = nullptr;
    QTableWidget  *m_registerTable = nullptr;
};

#endif // MONITORPAGE_H
