#include "ModelUpdater.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModelUpdater w;
    w.show();
    
    return a.exec();
}
