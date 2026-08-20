#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置 Fusion 风格 —— 工控软件常用，跨平台一致性好
    QApplication::setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}