#include "dialog.h"
#include <QApplication>
#include "tSettings.h"
//#include <crtdbg.h>

tSettings my_settings;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString S=a.applicationDirPath()+QDir::separator()+"Settings.ini";
    my_settings.SetFilePath(S);
    Dialog w;
    w.show();
//    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    return a.exec();
}
