#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    if(argc>1){
        for(int i=1;i<argc;++i){
            w.openFile(QString(argv[i]));
        }
    }
    return a.exec();
}
