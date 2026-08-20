#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QTabWidget;
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
    // 后续逐步添加成员变量
};

#endif // MAINWINDOW_H