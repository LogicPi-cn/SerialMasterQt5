#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Splash Screen On
    QPixmap pixmap(":/icon/LogicPi.cn.png");
    QSplashScreen splash(pixmap);
    splash.show();

    // Delay
    //    QDateTime n = QDateTime::currentDateTime();
    //    QDateTime now;
    //    do {
    //        now = QDateTime::currentDateTime();
    //    } while (n.secsTo(now) <= 0.5);

    // Main Window
    MainWindow w;
    w.show();

    // Close
    splash.finish(&w);

    return a.exec();
}
