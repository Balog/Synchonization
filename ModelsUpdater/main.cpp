//#include "ModelsUpdater.h"
#include <QApplication>
#include "Zast.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Zast w;
    w.show();
    
    return a.exec();
}
