#include <QApplication>
#include <mainwindow.h>

//layout dir path
QString layoutfile;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //a.setStyle("cleanlooks");
    MainWindow w;
    w.show();
    return a.exec();
}
