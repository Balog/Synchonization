#include "tExportMain.h"

tExportMain::tExportMain(QObject *parent) :
    QObject(parent),MModule(new MainModule)
{
}
//----------------------------------------------------------
tExportMain::~tExportMain()
{
    delete MModule;
}
