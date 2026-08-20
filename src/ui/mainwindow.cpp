#include "mainwindow.h"

#include <QTabWidget>
#include <QStatusBar>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    initConnect();
    initData();
}

MainWindow::~MainWindow()
{
    // 对象树自动析构，这里一般不需要手动 delete
}

void MainWindow::initUI()
{
    setWindowTitle("工业监控上位机 v1.0");
    resize(1200, 800);

    // 状态栏 —— 后面用来显示连接状态
    statusBar()->showMessage("就绪");
}

void MainWindow::initConnect()
{
    // 后面逐步添加信号槽连接
}

void MainWindow::initData()
{
    // 后面加载 ini 配置、初始化数据
}