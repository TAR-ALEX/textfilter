#include <QApplication>
#include <mainwindow.h>

//layout dir path
QString layoutfile;

int main(int argc, char *argv[])
{
    putenv((char *)"QT_AUTO_SCREEN_SCALE_FACTOR=1");
    QApplication a(argc, argv);
    // a.setStyle("cleanlooks");
    MainWindow w;
    w.show();
    return a.exec();
}
