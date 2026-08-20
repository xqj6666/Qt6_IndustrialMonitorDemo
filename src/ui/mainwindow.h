#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QTabWidget;
class QSplitter;
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindow)

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void initUI();       // 初始化界面
    void initConnect();  // 初始化信号槽连接
    void initData();     // 初始化数据、加载配置

private:
    QSplitter   *m_mainSplitter = nullptr; //主分割器(上下分割)
    QTabWidget  *m_tabWidget    = nullptr;//多页面容器
    QTextEdit   *m_logView      = nullptr;//日志显示区域
    QLabel      *m_statusLabel  = nullptr;//状态连信息标签
};

#endif // MAINWINDOW_H













