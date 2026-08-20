#ifndef MONITORPAGE_H
#define MONITORPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

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

private:
    QLabel* m_placeholderLabel = nullptr;//占位标签,后续替换成真实控件
};

#endif // MONITORPAGE_H
