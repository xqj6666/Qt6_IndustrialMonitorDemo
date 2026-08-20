#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
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
    QLabel *m_placeholderLabel = nullptr;
};

#endif // CONFIGPAGE_H