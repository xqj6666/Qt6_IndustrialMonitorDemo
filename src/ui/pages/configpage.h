//===配置界面：配置持久化，读取和保存====
#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
//整数微调框，可以限制范围
class QSpinBox;
class QPushButton;
class QGroupBox;
class QGridLayout;
//配置文件类，不用自己写文件解析
class QSettings;
QT_END_NAMESPACE

class ConfigPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ConfigPage)

public:
    explicit ConfigPage(QWidget *parent = nullptr);
    ~ConfigPage() override;

    //获取配置值的接口，供其他模块使用
    QString ipAddress()     const;
    quint16 port()          const;
    int     pollInterval()  const;

    //成功返回空字符，失败返回错误信息
    QString validateInput() const;

private:
    void initUI();
    void initConnect();

    void loadConfig();

private slots:
    void saveConfig();
    void resetConfig();

private:
    QLineEdit   *m_ipEdit            = nullptr;
    QSpinBox    *m_portSpin          = nullptr;
    QSpinBox    *m_pollIntervalSpin  = nullptr;
    QPushButton *m_saveBtn           = nullptr;
    QPushButton *m_resetBtn          = nullptr;

    //默认值常量
    static constexpr const char *DEFAULT_IP = "192.168.1.100";
    static constexpr int DEFAULT_PORT = 502;
    static constexpr int DEFAULT_POLL_INTERVAL =1000;
};

#endif // CONFIGPAGE_H















