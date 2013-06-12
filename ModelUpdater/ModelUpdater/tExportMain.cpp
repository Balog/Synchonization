#include "tExportMain.h"

tExportMain::tExportMain(QObject *parent) :
    QObject(parent), MModule(new MainModule())
{
    MModule->StartDatabase(true);
    qDebug() << "Конструктор ExportMain!";
    connect(MModule, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(MModule, SIGNAL(StartAutorizForm()), this, SLOT(OnStartAutorizForm()));
    connect(this, SIGNAL(SendAutorization(QString&,QString&,bool)), MModule, SLOT(OnSendAutorization(QString&,QString&, bool)));

    connect(MModule, SIGNAL(EndUpdatingFromServer(QList<CompareTableRec>,bool)),this,SLOT(OnEndUpdatingFromServer(QList<CompareTableRec>,bool)));
    connect(MModule, SIGNAL(SignalCountFiles(int)), this, SLOT(setValue(int)));
    connect(MModule, SIGNAL(EndTransactions()), this, SLOT(OnEndTransactions()));
    connect(MModule, SIGNAL(RebuildTrees(QList<CompareTableRec>)), this, SLOT(OnRebuildTrees(QList<CompareTableRec>)));
    connect(MModule, SIGNAL(DisconnectingFromServer()), this, SLOT(OnDisconnectingFromServer()));
    connect(MModule, SIGNAL(ErrorUserFolders(QString&,QString&)), this, SLOT(OnErrorUserFolders(QString&,QString&)));
    connect(MModule, SIGNAL(Update_Logins()), this, SLOT(OnUpdate_Logins()));
    connect(MModule, SIGNAL(ShowEditLogin(bool,bool)), this, SLOT(OnShowEditLogin(bool, bool)));

}
//----------------------------------------------------------
tExportMain::~tExportMain()
{
    delete MModule;
}
//----------------------------------------------------------
void tExportMain::OnDisconnectingFromServer()
{
    qDebug() << "tExportMain::OnDisconnectingFromServer";
    emit Disconnect();
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
//----------------------------------------------------------
void tExportMain::setValue(int _value)
{
    emit SignalCountFiles(_value);
}
//----------------------------------------------------------
void tExportMain::OnEndTransactions()
{
    emit EndTransactions();
}
//----------------------------------------------------------
void tExportMain::DeletingServerFile(const QString &_file_name)
{
    MModule->DeletingServerFile(_file_name);
}
//----------------------------------------------------------
void tExportMain::SendFile(const QString &_file_name)
{
    MModule->SendFile(_file_name);
}
//----------------------------------------------------------
int tExportMain::GetCountSendDelModels()
{
    return MModule->GetCountSendDelModels();
}
//----------------------------------------------------------
void tExportMain::StartSendDeleteFiles(const int _max_model)
{
    MModule->StartSendDeleteFiles(_max_model);
}
//----------------------------------------------------------
void tExportMain::ActualiseModel(const QString &_login, const qlonglong _num_model, const bool _from_server)
{
    MModule->ActualiseModel(_login, _num_model, _from_server);
}
//----------------------------------------------------------
QString tExportMain::VerifyCustomCopyPath(const QString& path) const
{
    return MModule->VerifyCustomCopyPath(path);
}
//----------------------------------------------------------
void tExportMain::RunGui(QByteArray& block)
{
    qDebug() << "tExportMain::RunGui";
    MModule->OnRunGui(block);
}
//----------------------------------------------------------
void tExportMain::OnRebuildTrees(QList<CompareTableRec> _list)
{
    qDebug() << "tExportMain::OnRebuildTrees";
//    emit RebuildTrees(_list);
    EndUpdatingFromServer(_list, true);
}
//----------------------------------------------------------
void tExportMain::Disconnecting()
{
    MModule->OnDisconnectingFromClient();
}
//----------------------------------------------------------
void tExportMain::SetServerParameters(const QString &_addr, const int _port)
{
    MModule->SetServerParameters(_addr, _port);

}
//----------------------------------------------------------
void tExportMain::StartServer()
{
    MModule->StartServer();
}
//----------------------------------------------------------
void tExportMain::GetServerParameters(QString &_addr, int& _port)
{
    MModule->GetServerParameters(_addr, _port);
}
//----------------------------------------------------------
void tExportMain::ReStartServer()
{
    delete MModule;
    MModule=new MainModule();
//    MModule->StartServer();
}
//----------------------------------------------------------
void tExportMain::GetFolderParameters(const QString& login, QString& roor_folder, QString& temp_folder, QString& mess)
{
    MModule->GetFolderParameters(login, roor_folder, temp_folder, mess);
    qDebug() << "tExportMain::GetFolderParameters" << roor_folder << temp_folder << mess;
}
//----------------------------------------------------------
void tExportMain::OnErrorUserFolders(QString& _login,QString& _mess)
{
    emit ErrorUserFolders(_login, _mess);
}
//----------------------------------------------------------
bool tExportMain::VerifyUserFolders(QString& _login, QString& _pr_folder, QString& _temp_folder, QString& _message)
{
    return MModule->VerifyUserFolders(_login, _pr_folder, _temp_folder, _message);
}
//----------------------------------------------------------
void tExportMain::SaveFoldersToLoginsTable(const QString& _login, const QString& _pr_folder, const QString& _temp_folder)
{
    MModule->SaveFoldersToLoginsTable(_login, _pr_folder, _temp_folder);
}
//----------------------------------------------------------
void tExportMain::SaveFoldersToSettings(const QString& _user_login)
{
    MModule->SaveFoldersToSettings(_user_login);
}
//----------------------------------------------------------
void tExportMain::GetPermissionsUser(const QString& _login, bool& _is_admin_user, bool& _is_writable_user)
{
    MModule->GetPermissionsUser(_login, _is_admin_user, _is_writable_user);
}
//----------------------------------------------------------
void tExportMain::RefreshModelsFiles()
{
    MModule->RefreshModelsFiles();
}
//----------------------------------------------------------
bool tExportMain::removeFolder(const QDir & _dir, const bool _del_dir)
{
    return MModule->removeFolder(_dir, _del_dir);
}
//----------------------------------------------------------
void tExportMain::SendDeleteLogin(int _num)
{
    MModule->SendDeleteLogin(_num);
}
//----------------------------------------------------------
void tExportMain::SaveLoginWritable(const QStandardItemModel *_model, const int _row)
{
    MModule->SaveLoginWritable(_model, _row);
}
//----------------------------------------------------------
void tExportMain::GetLoginsModel(QStandardItemModel *model)
{
    MModule->GetLoginsModel(model);
}
//----------------------------------------------------------
bool tExportMain::VerPassword(const QString &login, const QString &_pass)
{
    return MModule->VerPassword(login, _pass);
}
//----------------------------------------------------------
qlonglong tExportMain::GetNumLogin(const int _row) const
{
    return MModule->GetNumLogin(_row);
}
//----------------------------------------------------------
qlonglong tExportMain::GetNumLogin(const QString &_login) const
{
    return MModule->GetNumLogin(_login);
}
//----------------------------------------------------------
void tExportMain::SaveReadPermission(const QString &_login, const qlonglong _mod_num, const bool _state)
{
    MModule->SaveReadPermission(_login, _mod_num, _state);
}
//----------------------------------------------------------
void tExportMain::ResetFoundModelAdmin()
{
    MModule->ResetFoundModelAdmin();
}
//----------------------------------------------------------
bool tExportMain::NextModelAdmin() const
{
    return MModule->NextModelAdmin();
}
//----------------------------------------------------------
QStringList tExportMain::NextStructListModelAdmin(const QString &_login, bool &_read, qlonglong &_server_num) const
{
    return MModule->NextStructListModelAdmin(_login, _read, _server_num);
}
//----------------------------------------------------------
void tExportMain::SavePermissionsToServer(const qlonglong _num_login)
{
    MModule->SavePermissionsToServer(_num_login);
}
//----------------------------------------------------------
void tExportMain::OnUpdate_Logins()
{
    emit Update_Logins();
}
//----------------------------------------------------------
void tExportMain::SendLoginPassword(const QString &_login, const QString &_password, const int _row, const bool _new_user)
{
    MModule->SendLoginPassword(_login, _password, _row, _new_user);
}
//----------------------------------------------------------
void tExportMain::OnShowEditLogin(bool Visible, bool Modal)
{
    emit ShowEditLogin(Visible, Modal);
}
//----------------------------------------------------------
QStringList tExportMain::GetListErrors()
{
    return MModule->GetListErrors();
}
//----------------------------------------------------------
void tExportMain::ClearListErrors()
{
    MModule->ClearListErrors();
}
