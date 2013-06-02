#include "tExportZast.h"
#include<QDebug>

tExportZast::tExportZast(QObject *parent) :
    QObject(parent), zast_mod(new tZastModule)
{
    connect(zast_mod, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));

}
//-------------------------------------------------------
tExportZast::~tExportZast()
{
    delete zast_mod;
}
//-------------------------------------------------------
void tExportZast::StartFindServer()
{
    zast_mod->StartFindServer();
//    connect(zast_mod, SIGNAL(ServerFinding()), this, SLOT(OnServerFinding()));
}
//-------------------------------------------------------
void tExportZast::OnFindServer(bool ok)
{
    qDebug() << ok;
    if(ok)
    {

      log.Write(QString("tExportZast /t OnFindServerFalse /t СЕРВЕР НАЙДЕН"));
    }
    else
    {
    log.Write(QString("tExportZast /t OnFindServerFalse /t СЕРВЕР НЕ НАЙДЕН"));
}

    emit FindServer(ok);

}
//-------------------------------------------------------
bool tExportZast::GetConnect()
{
    return zast_mod->GetConnect();
}
//-------------------------------------------------------
//void tExportZast::OnServerFinding()
//{
//    qDebug() << "Finding";
//}
