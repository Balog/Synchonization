#include "tAutoExportMain.h"

tAutoExportMain::tAutoExportMain(QObject *parent) :
    QObject(parent), MModule(new MainModule())
{
    qDebug() << "Конструктор AutoExportMain";
    MModule->StartDatabase(false);

    connect(MModule, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(MModule, SIGNAL(ErrorFolders(QString&)), this, SLOT(OnErrorFolders(QString&)));
    connect(MModule, SIGNAL(FoldersOk()), this, SLOT(OnFoldersOk()));
    connect(MModule, SIGNAL(SendModels(QList<tServerModel>&)), this, SLOT(OnSendModels(QList<tServerModel>&)));
    connect(this, SIGNAL(SendAutorization(QString&,QString&,bool)), MModule, SLOT(OnSendAutorization(QString&,QString&, bool)));
    connect(MModule, SIGNAL(EndTransactions()), this, SLOT(OnEndTransactions()));
}
//----------------------------------------------------------
tAutoExportMain::~tAutoExportMain()
{
    qDebug() << "Удаляю MModule";
    delete MModule;
}

//----------------------------------------------------------
void tAutoExportMain::OnFindServer(bool ok)
{
    qDebug() << "Автопределение сервера " << ok;

    if(ok)
    {
        qDebug() << "Сервер найден";
        //Продолжим работу, нужна авторизация
        //запросить в MModule пароль и логин из файла настроек если такие данные там есть
        //потом их можно откорректироватьесли надо
        QString login="";
        QString password="";
        MModule->GetAutorizationInfo(login, password);

        qDebug() << "логин и пароль " << login << password;

        //тут можно переприсвоить значение login и password

        emit FindServer(login, password);
    }
    else
    {
        qDebug() << "Сервер не найден";
        QString error="Сервер не найден";
        emit Error(error);
    }

}
//----------------------------------------------------------
void tAutoExportMain::VerifyUserFolders()
{
    MModule->VerifyUserFolders();
}
//----------------------------------------------------------
void tAutoExportMain::OnErrorFolders(QString& error)
{
    emit Error(error);
}
//----------------------------------------------------------
void tAutoExportMain::OnFoldersOk()
{
    emit FoldersOk();
}
//----------------------------------------------------------
void tAutoExportMain::OnListFiles()//const QStringList& _auto_models
{

//    ModelsList=_auto_models;
    MModule->OnListFiles();
}
//----------------------------------------------------------
void tAutoExportMain::OnSendModels(QList<tServerModel>& _Model)
{
    emit SendModels(_Model);//
}
//----------------------------------------------------------
void tAutoExportMain::Autorization(QString& _login, QString& _password)
{
    qDebug() << "tExportMain::OnSendAutorization" << _login << _password;
    emit SendAutorization(_login, _password, false);
}
//----------------------------------------------------------
QStringList tAutoExportMain::ReadAutoUserModels()
{
    QStringList list=MModule->ReadAutoUserModels();
    qDebug() << "Число моделей:" << list.size();
    return list;
}
//----------------------------------------------------------
void tAutoExportMain::ReceivingModels(QList<tServerModel> &_models)
{
    MModule->ReceivingModels(_models);
}
//----------------------------------------------------------
void tAutoExportMain::OnEndTransactions()
{
    emit EndTransactions();
}
