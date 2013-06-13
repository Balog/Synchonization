#include "tAutoExportMain.h"

tAutoExportMain::tAutoExportMain(QObject *parent) :
    QObject(parent), MModule(new MainModule())
{
    qDebug() << "Конструктор AutoExportMain";
    MModule->StartDatabase(false);

    connect(MModule, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(MModule, SIGNAL(ErrorFolders(QString&)), this, SLOT(OnErrorFolders(QString&)));
    connect(MModule, SIGNAL(FoldersOk()), this, SLOT(OnFoldersOk()));
    connect(MModule, SIGNAL(SendModels(QList<tServerModel>)), this, SLOT(OnSendModels(QList<tServerModel>)));
    connect(this, SIGNAL(SendAutorization(QString&,QString&,bool)), MModule, SLOT(OnSendAutorization(QString&,QString&, bool)));
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
void tAutoExportMain::OnListFiles()
{
    //Нужно передавать список моделей, предварительно прочитав их из файла.
    QStringList ModelsList=MModule->ReadAutoUserModels();
    MModule->OnListFiles(ModelsList);
}
//----------------------------------------------------------
void tAutoExportMain::OnSendModels(QList<tServerModel> Model)
{
    emit SendModels(Model);
}
//----------------------------------------------------------
void tAutoExportMain::Autorization(QString& _login, QString& _password)
{
    qDebug() << "tExportMain::OnSendAutorization" << _login << _password;
    emit SendAutorization(_login, _password, false);
}
