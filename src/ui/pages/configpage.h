#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QGroupBox;
class QGridLayout;
QT_END_NAMESPACE

class ConfigPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ConfigPage)

public:
    explicit ConfigPage(QWidget *parent = nullptr);
    ~ConfigPage() override;

private:
    void initUI();
    void initConnect();

private:
    QLineEdit   *m_ipEdit            = nullptr;//Ip地址输入框
    QSpinBox    *m_portSpin          = nullptr;//端口号输入框
    QSpinBox    *m_pollIntervalSpin  = nullptr;//轮询周期输入框
    QPushButton *m_saveBtn           = nullptr;//保存配置按钮
    QPushButton *m_resetBtn          = nullptr;//恢复默认按钮
};

#endif // CONFIGPAGE_H