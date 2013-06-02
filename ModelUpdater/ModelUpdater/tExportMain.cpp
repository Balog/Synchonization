#include "tExportMain.h"

tExportMain::tExportMain(QObject *parent) :
    QObject(parent)
{
    MModule=new MainModule();
    qDebug() << "Конструктор ExportMain";
    connect(MModule, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(MModule, SIGNAL(StartAutorizForm()), this, SLOT(OnStartAutorizForm()));
//    StartFindServer();
}
//----------------------------------------------------------
tExportMain::~tExportMain()
{
    delete MModule;
}
//----------------------------------------------------------
void tExportMain::StartFindServer()
{
//    connect(zast_mod, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
//    zast_mod->StartFindServer();
//    connect(zast_mod, SIGNAL(ServerFinding()), this, SLOT(OnServerFinding()));

}
//----------------------------------------------------------
void tExportMain::OnFindServer(bool ok)
{
    qDebug() << "Определение сервера " << ok;
    emit FindServer(ok);
}
//----------------------------------------------------------
void tExportMain::OnStartAutorizForm()
{
    emit StartAutorizForm();
}
