#include "ModelUpdater.h"
#include <QApplication>
#include "tSettings.h"
#include <QDir>

tSettings my_settings;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString S=a.applicationDirPath()+QDir::separator()+"Settings.ini";
    my_settings.SetFilePath(S);
    ModelUpdater w;
    w.show();
    
    return a.exec();
}
