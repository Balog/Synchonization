#include "tExportMain.h"

tExportMain::tExportMain(QObject *parent) :
    QObject(parent), MModule(new MainModule())
{
    qDebug() << "Конструктор ExportMain";
    connect(MModule, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(MModule, SIGNAL(StartAutorizForm()), this, SLOT(OnStartAutorizForm()));
    connect(this, SIGNAL(SendAutorization(QString&,QString&,bool)), MModule, SLOT(OnSendAutorization(QString&,QString&, bool)));

    connect(MModule, SIGNAL(EndUpdatingFromServer(QList<CompareTableRec>,bool)),this,SLOT(OnEndUpdatingFromServer(QList<CompareTableRec>,bool)));

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
//----------------------------------------------------------
void tExportMain::OnSendAutorization(QString& _login, QString& _password, bool _mod_folder)
{
    qDebug() << "tExportMain::OnSendAutorization" << _login << _password;
    emit SendAutorization(_login, _password, _mod_folder);
}
//----------------------------------------------------------
void tExportMain::OnEndUpdatingFromServer(QList<CompareTableRec> _list_compare, bool _rebuild)
{
    qDebug() << "tExportMain::OnEndUpdatingFromServer";
    emit EndUpdatingFromServer(_list_compare, _rebuild);
}
//----------------------------------------------------------
//tDatabaseOp* tExportMain::GetDatabase()
//{
//    return MModule->GetDatabase();
//}
//----------------------------------------------------------
QString tExportMain::GetTemp()
{
    return MModule->GetTemp();
}
//----------------------------------------------------------
void tExportMain::GetModelInfo(const qlonglong _loc_num, QString& _title_model, QString &_description, QList<tFile> &_files_model, QStringList& _previews)
{
    MModule->GetModelInfo(_loc_num, _title_model, _description, _files_model, _previews);
}

//----------------------------------------------------------
QString tExportMain::LoginFromNum(const qlonglong _num_login) const
{
    return MModule->LoginFromNum(_num_login);
}
//----------------------------------------------------------
QString tExportMain::GetServerModelPath(const qlonglong _num_server) const
{
    return MModule->GetServerModelPath(_num_server);
}
//----------------------------------------------------------
void tExportMain::GetServerModelInfo(const qlonglong _serv_num, QString& _title_model, QString &_description, QList<tFile> &_files_model)
{
    MModule->GetServerModelInfo(_serv_num, _title_model, _description, _files_model);
}
//----------------------------------------------------------
void tExportMain::SaveDescription(const qlonglong _num, const QString &_text, QString &_file_name)
{
    MModule->SaveDescription(_num, _text, _file_name);
}
//----------------------------------------------------------
void tExportMain::UpdateInfoData(const qlonglong _num_model, const QString &_file_name, const QString &_hash, const QDateTime &_last_mod)
{
    MModule->UpdateInfoData(_num_model, _file_name, _hash, _last_mod);
}
//----------------------------------------------------------
QString tExportMain::GetRoot()
{
    return MModule->GetRoot();
}
//----------------------------------------------------------
void tExportMain::OnListFilesLocal()
{
    MModule->OnListFilesLocal();
}
//----------------------------------------------------------
void tExportMain::WriteToCompareTablesToTree(const QString& _login)
{
    MModule->WriteToCompareTablesToTree(_login);
}
//----------------------------------------------------------
QList<CompareTableRec> tExportMain::AnalyzeCompareAll()
{
    return MModule->AnalyzeCompareAll();
}
//----------------------------------------------------------
void tExportMain::AddFilesToModelsStruct(QList<CompareTableRec> &comp_models)
{
    MModule->AddFilesToModelsStruct(comp_models);
}
//----------------------------------------------------------
void tExportMain::SetTransactionFlag(const bool _flag)
{
    MModule->SetTransactionFlag(_flag);
}
//----------------------------------------------------------
void tExportMain::ClearConveyor()
{
    MModule->ClearConveyor();
}
//----------------------------------------------------------
void tExportMain::DeletingLocalFile(const QString& _file_name)
{
    MModule->DeletingLocalFile(_file_name);
}
//----------------------------------------------------------
void tExportMain::ReceiveFile(const QString& _file_name)
{
    MModule->ReceiveFile(_file_name);
}
//----------------------------------------------------------
void tExportMain::GetServerListPreviews(const qlonglong _server_num_model, QStringList &_list)
{
    MModule->GetServerListPreviews(_server_num_model, _list);
}
//----------------------------------------------------------
void tExportMain::GetServerListFiles(const qlonglong _num_server_model, QStringList &_list)
{
    MModule->GetServerListFiles(_num_server_model, _list);
}
//----------------------------------------------------------
int tExportMain::GetCountRecDelModels()
{
    return MModule->GetCountRecDelModels();
}
//----------------------------------------------------------
void tExportMain::StartReceiveDeleteFiles(const QString &_root, int _custom_copy, int max_model)
{
    MModule->StartReceiveDeleteFiles(_root, _custom_copy, max_model);
}




