#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("streamx3");
    a.setApplicationName("QLayoutEditor");
    a.setApplicationVersion("0.1.0");
    a.setWindowIcon(QIcon(":/QLayoutEditor.svg"));
    MainWindow w;
    w.show();
    return a.exec();
}
