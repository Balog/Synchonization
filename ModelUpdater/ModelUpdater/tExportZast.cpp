#include "tExportZast.h"

tExportZast::tExportZast(QObject *parent) :
    QObject(parent), zast_mod(new tZastModule)
{
    connect(zast_mod, SIGNAL(FindServerFalse()), this, SLOT(OnFindServerFalse()));
}
//-------------------------------------------------------
tExportZast::~tExportZast()
{
    delete zast_mod;
}
//-------------------------------------------------------
void tExportZast::OnFindServerFalse()
{
    emit FindServerFalse();
}
//-------------------------------------------------------
bool tExportZast::GetConnect()
{
    return zast_mod->GetConnect();
}
