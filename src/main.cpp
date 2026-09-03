#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置 Fusion 风格 —— 工控软件常用，跨平台一致性好
    QApplication::setStyle("Fusion");

    //===全局深色主题===
    QPalette darkPalette;

    // 窗口背景
    darkPalette.setColor(QPalette::Window,          QColor(45, 45, 48));
    // 窗口文字
    darkPalette.setColor(QPalette::WindowText,      QColor(220, 220, 220));
    // 输入框、表格等控件背景
    darkPalette.setColor(QPalette::Base,            QColor(30, 30, 30));
    // 输入框、表格等控件的交替行背景
    darkPalette.setColor(QPalette::AlternateBase,   QColor(45, 45, 48));
    // 工具提示背景
    darkPalette.setColor(QPalette::ToolTipBase,     QColor(30, 30, 30));
    // 工具提示文字
    darkPalette.setColor(QPalette::ToolTipText,     QColor(220, 220, 220));
    // 文字
    darkPalette.setColor(QPalette::Text,            QColor(220, 220, 220));
    // 按钮背景
    darkPalette.setColor(QPalette::Button,          QColor(55, 55, 58));
    // 按钮文字
    darkPalette.setColor(QPalette::ButtonText,      QColor(220, 220, 220));
    // 选中项背景
    darkPalette.setColor(QPalette::Highlight,       QColor(0, 120, 215));
    // 选中项文字
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    // 禁用状态文字
    darkPalette.setColor(QPalette::Disabled,        QPalette::Text, QColor(128, 128, 128));
    darkPalette.setColor(QPalette::Disabled,        QPalette::ButtonText, QColor(128, 128, 128));
    // 分组框标题
    darkPalette.setColor(QPalette::BrightText,      QColor(255, 0, 0));
    // 链接
    darkPalette.setColor(QPalette::Link,            QColor(0, 120, 215));

    app.setPalette(darkPalette);

    // 全局样式表补充（QPalette 覆盖不到的部分）
    app.setStyleSheet(
        "QGroupBox {"
        "    border: 1px solid #555;"
        "    border-radius: 4px;"
        "    margin-top: 8px;"
        "    padding-top: 16px;"
        "    color: #dcdcdc;"
        "    font-weight: bold;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 4px;"
        "}"
        "QTabWidget::pane {"
        "    border: 1px solid #555;"
        "}"
        "QTabBar::tab {"
        "    background: #3c3c3c;"
        "    color: #dcdcdc;"
        "    padding: 6px 16px;"
        "    border: 1px solid #555;"
        "    border-bottom: none;"
        "}"
        "QTabBar::tab:selected {"
        "    background: #2d2d30;"
        "    color: #ffffff;"
        "}"
        "QHeaderView::section {"
        "    background-color: #3c3c3c;"
        "    color: #dcdcdc;"
        "    padding: 4px;"
        "    border: 1px solid #555;"
        "}"
        "QStatusBar {"
        "    background: #007acc;"
        "    color: #ffffff;"
        "}"
        "QPushButton {"
        "    padding: 4px 12px;"
        "    border: 1px solid #555;"
        "    border-radius: 3px;"
        "    background: #3c3c3c;"
        "    color: #dcdcdc;"
        "}"
        "QPushButton:hover {"
        "    background: #505054;"
        "}"
        "QPushButton:pressed {"
        "    background: #007acc;"
        "}"
        "QPushButton:disabled {"
        "    background: #2d2d2d;"
        "    color: #808080;"
        "}"
        "QSplitter::handle {"
        "    background: #555;"
        "}"
        );

    MainWindow window;
    window.show();

    return app.exec();
}