
//#include <stdlib.h>
//#include <crtdbg.h>

#include "Zast.h"
#include <QApplication>
#include "tSettings.h"

tSettings my_settings;
int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    QString S=a.applicationDirPath()+QDir::separator()+"Settings.ini";
    my_settings.SetFilePath(S);
    Zast w;
    w.show();
    
//    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    return a.exec();
}
