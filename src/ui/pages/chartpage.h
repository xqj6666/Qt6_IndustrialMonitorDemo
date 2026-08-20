#ifndef CHARTPAGE_H
#define CHARTPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class ChartPage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ChartPage)

public:
    explicit ChartPage(QWidget* parent = nullptr);
    ~ChartPage() override;

private:
    void initUI();
    void initConnect();

private:
    QLabel* m_placeholderLabel = nullptr;
};

#endif // CHARTPAGE_H
