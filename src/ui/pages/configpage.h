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
class QSettings;//配置文件类，不用自己写文件解析
QT_END_NAMESPACE

class ConfigPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ConfigPage)

public:
    explicit ConfigPage(QWidget *parent = nullptr);
    ~ConfigPage() override;

    //获取配置值的接口，供其他模块使用
    QString ipAddress() const;
    quint16 port() const;
    int pollInterval() const;

private:
    void initUI();
    void initConnect();
    void loadConfig();
    void saveConfig();
    void resetConfig();

private:
    QLineEdit   *m_ipEdit            = nullptr;//Ip地址输入框
    QSpinBox    *m_portSpin          = nullptr;//端口号输入框
    QSpinBox    *m_pollIntervalSpin  = nullptr;//轮询周期输入框
    QPushButton *m_saveBtn           = nullptr;//保存配置按钮
    QPushButton *m_resetBtn          = nullptr;//恢复默认按钮

    //默认值常量
    static constexpr const char* DEFAULT_IP = "192.168.1.100";
    static constexpr int DEFAULT_PORT = 502;
    static constexpr int DEFAULT_POLL_INTERVAL =1000;
};

#endif // CONFIGPAGE_H















