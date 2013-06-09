#include "MainModule.h"
#include<QMessageBox>
#include<tSettings.h>

tSettings my_settings;

MainModule::MainModule(QObject *parent) :
    QObject(parent), db_op(new tDatabaseOp)
{
    IsRequeryServerModel=false;
    qDebug() << "Конструктор MainModule";
    log.Write(QString("Конструктор MainModule"));
    QString S=QCoreApplication::applicationDirPath()+QDir::separator()+"Settings.ini";
    my_settings.SetFilePath(S);

    timer1=new QTimer();

    connect(timer1, SIGNAL(timeout()), this, SLOT(ContinueStart()));
    timer1->start(3000);

    timer=new QTimer();

    connect(timer, SIGNAL(timeout()),this, SLOT(OnFindServerFalse()));

    timer->setInterval(10000);
    timer->start();

    qDebug() << "База данных" << db_op;
    mod_conv= new tModelsConveyor(this, db_op);

    connect(mod_conv, SIGNAL(SignalCountFiles(int)), this, SLOT(setValue(int)));
    connect(mod_conv, SIGNAL(EndTransactionsMain()), this, SLOT(OnEndTransactions()));
    connect(this, SIGNAL(RunGui(QByteArray&)),mod_conv, SLOT(OnRunGui(QByteArray&)));
    connect(mod_conv, SIGNAL(Disconnecting()), this, SLOT(OnDisconnectingFromServer()));

//    connect(this, SIGNAL(FindServerTrue()), zast_mod, SLOT(OnTimerTrue()));

}
//---------------------------------------------------------
void MainModule::OnDisconnectingFromServer()
{
    qDebug() << "MainModule::OnDisconnectingFromServer";
    emit DisconnectingFromServer();
}
//---------------------------------------------------------
void MainModule::OnFindServerFalse()
{
    qDebug() << "Время истекло";
    tLog log;
    log.Write(tr("MainForm \t OnFindServerFalse \t Время истекло"));
    emit FindServer(false);
}
//---------------------------------------------------------
void MainModule::ContinueStart()
{
    delete timer1;
    timer1=NULL;
    mod_conv->StartServer(my_settings.GetServerAddr(), my_settings.GetServerPort());
}
//---------------------------------------------------------
MainModule::~MainModule()
{
//    delete form_new_path;
//    delete login_pass;

//    delete sLM_Logins;
//    delete tableModel;

//    delete adm_tree_model;
//    delete read_tree_model;
//    delete write_tree_model;
//    delete fProgress;
//    delete zast_mod;
//    zast_mod=NULL;
    delete timer;
    timer=NULL;

    delete timer1;
    timer1=NULL;

    delete mod_conv;
    mod_conv=NULL;

    delete db_op;
    db_op=NULL;

//    delete table_files_model;
}

//---------------------------------------------------------
void MainModule::OnAutorizStart()
{
    tLog log;
    log.Write(tr("MainForm \t OnAutorizStart \t Подключение разрешено"));

    delete timer;
    timer=NULL;

    mod_conv->ReceiveLoginsTable();

    emit FindServer(true);
}
//---------------------------------------------------------
void MainModule::OnSendAutorization(QString& _login, QString& _password, bool _mod_folder)
{
    qDebug() << "MainModule::OnSendAutorization" << _login << _password;
    user_login=_login;
    mod_conv->SetLogin(_login);
    modify_folder=_mod_folder;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SendAutorization");
    out << tr("SendAutorization");
    out << _login;
    out << _password;

    emit RunGui(block);
}
//---------------------------------------------------------
bool MainModule::GetIsTransaction()
{
    return mod_conv->GetIsTransaction();
}
//---------------------------------------------------------
void MainModule::ViewError(const int _num_error, const QString& _error, const QString &_detail, const QString& _client_detail)
{
    //Вообще будет выводиться куда-то на панель а пока так
    QMessageBox MB;
    MB.setText(_error+"\n"+_detail+"\n"+_client_detail);
    MB.setWindowTitle(QString::fromUtf8("Ошибка ")+QString::number(_num_error));
    MB.exec();
}
//---------------------------------------------------------
void MainModule::UpdateLogins()
{

//    db_op->GetLoginsModel(tableModel);

//    ui->lvLogins->setModel(tableModel);
    emit Update_Logins();
}
//---------------------------------------------------------
bool MainModule::VerifyUserFolders()
{
    qDebug() << "MainModule::VerifyUserFolders" << modify_folder;
    QString message="";
    QString project_folder="";
    QString temp_folder="";
    if(!modify_folder && db_op->VerifyUserFolders(user_login, project_folder, temp_folder, message))
    {
        qDebug() << "MainModule::VerifyUserFolders" << user_login << project_folder << temp_folder << message;
        return true;
    }
    else
    {
        qDebug() << "ErrorUserFolders" << user_login << message;
        emit ErrorUserFolders(user_login, message);
//        form_new_path->SetDatabase(db_op);
//        form_new_path->SetLogin(_user_login);
//        form_new_path->SetMessage(message);
//        form_new_path->setVisible(true);
        return false;
    }

}
//----------------------------------------------------------
bool MainModule::VerifyUserFolders(QString& _login, QString& _pr_folder, QString& _temp_folder, QString& _message)
{
    return db_op->VerifyUserFolders(_login, _pr_folder, _temp_folder, _message);
}
//----------------------------------------------------------
void MainModule::OnContinueStart()
{
    qDebug() << "Авторизация принята";
    db_op->SaveFoldersToSettings(user_login);

    //Это все можно сделать из главной формы по окончании старта
//    ui->leRoot->setText(my_settings.GetRoot());
//    ui->leTemp->setText(my_settings.GetTemp());

//    bool is_admin_user=false;
//    bool is_writable_user=false;
//    db_op->GetPermissionsUser(user_login, is_admin_user, is_writable_user);

    //Это можно сделать по окончании старта, проверить переменные и все сделать в главной форме
//    if(!is_admin_user)
//    {
//        ui->tabWidget->setTabEnabled(2, false);
//    }
//    else
//    {
//        ui->tabWidget->setTabEnabled(2, true);
//    }

//    if(!is_writable_user)
//    {
//        ui->tabWidget->setTabEnabled(1, false);
//    }
//    else
//    {
//        ui->tabWidget->setTabEnabled(1, true);
//    }
//    ui->tabWidget->setCurrentIndex(0);

    db_op->RefreshModelsFiles();

    OnListFilesLocal();
    OnListFiles();

    qDebug() << "запрос обновления моделей";
}
//----------------------------------------------------------
void MainModule::CancelAllOperations()
{
    OnClearSendAll();
    OnClearDelete();
    OnReceiveClear();
    OnClearDeleteLocal();

    mod_conv->CancelOperations();
}
//----------------------------------------------------------
void MainModule::OnListFilesLocal()
{
    db_op->RefreshModelsFiles();

    VerifyLastTable(user_login);
}
//---------------------------------------------------------------------
void MainModule::OnListFiles()
{
    tLog log;
    log.Write(tr("MainForm \t OnListFiles \t Запрос с главной формы GetListModels через RunGui"));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GetListModels");
    out << tr("GetListModels");

    emit RunGui(block);
}
//---------------------------------------------------------------------
void MainModule::OnClearSendAll()
{
    listSend.clear();
}
//---------------------------------------------------------------------
void MainModule::OnClearDelete()
{
    listDel.clear();
}
//---------------------------------------------------------------------
void MainModule::OnReceiveClear()
{
    listRec.clear();
}
//---------------------------------------------------------------------
void MainModule::OnClearDeleteLocal()
{
    listDelLoc.clear();

}
//---------------------------------------------------------------------
void MainModule::VerifyLastTable(const QString& _user_login)
{
    //Проверить если есть такие модели в таблице Last
    //которых уже нет ни в серверной ни в локальной папке
    db_op->VerifyLastTable(_user_login);

}
//----------------------------------------------------------
void MainModule::SaveServerModelFiles(QByteArray &_block)
{
//    //распарсить переданый блок и записать в папки серверного состояния моделей и их файлов
    db_op->SaveServerModelFiles(_block);
}
//----------------------------------------------------------
void MainModule::CorrectLastSynch(const bool _server)
{

    mod_conv->MarkLastTables(_server, user_login);

    db_op->ExecUpdateLastSynch(_server, user_login);
    mod_conv->ClearAllList();
}
//----------------------------------------------------------
void MainModule::EndUpdateServerModel(const bool _rebuild)
{
    qDebug() << "MainModule::EndUpdateServerModel";

    //формирование дерева чтения по полученым и имеющимся данным после обновления данных с сервера

    OnListFilesLocal();

    BuildingTree(user_login);

    emit EndUpdatingFromServer(list_compare, _rebuild);
//emit retEndUpdateServerModel(_rebuild);
}
//----------------------------------------------------------
void MainModule::BuildingTree(const QString& _user_login)
{
    qDebug() << "MainModule::BuildingTree";
    OnListFilesLocal();

    //В таблицу CompareTablesToTree из локальной, ласт и серверной таблиц занести ориентируясь на Struct суммарные хеши моделей
    db_op->WriteToCompareTablesToTree(_user_login);

    //Провести анализ на направление изменений моделей в целом
    list_compare=db_op->AnalyzeCompareAll();

    //добавляем к стректуре моделей различающиеся файлы
    db_op->AddFilesToModelsStruct(list_compare);



}
//----------------------------------------------------------
void MainModule::RegisterUser(const qlonglong _s_num)
{
    //Получше напишу позже, это пока не так важно

    db_op->SaveLoginPassword(login, password, new_user, _s_num);

    //скрытие формы редактирования пользователя
    emit ShowEditLogin(false, false);
//    login_pass->setModal(false);
//    login_pass->setVisible(false);
//    login_pass->ClearAll();

UpdateLogins();
}
//----------------------------------------------------------
void MainModule::DeleteUser(const qlonglong _s_num)
{
    db_op->DeleteLogin(_s_num);

    emit ShowEditLogin(false, false);
//    login_pass->setModal(false);
//    login_pass->setVisible(false);
//    login_pass->ClearAll();

    UpdateLogins();
}
//----------------------------------------------------------
void MainModule::UpdateLoginsTable(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateLogins(_block);

    UpdateLogins();

    StartAutoriz();
}
//----------------------------------------------------------
void MainModule::StartAutoriz()
{
    //показ формы авторизации
    tLog log;
    log.Write(tr("показ формы авторизации"));
    emit StartAutorizForm();
}
//----------------------------------------------------------
void MainModule::UpdateModelRead(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateModelRead(_block);

    emit VisibleMain(true);
//    this->setVisible(true);
}
//----------------------------------------------------------
void MainModule::InternalCallBuildingTree()
{
    if(!GetIsTransaction())
    {
        qDebug() << "MainModule::InternalCallBuildingTree";
        BuildingTree(user_login);

        emit RebuildTrees(list_compare);
//        ConstructTree(Read, list_compare);
//        ConstructTree(Write, list_compare);
    }
}
//----------------------------------------------------------
//tDatabaseOp* MainModule::GetDatabase()
//{
//    return db_op;
//}
//----------------------------------------------------------
QString MainModule::GetTemp()
{
    return my_settings.GetTemp();
}
//----------------------------------------------------------
void MainModule::GetModelInfo(const qlonglong _loc_num, QString& _title_model, QString &_description, QList<tFile> &_files_model, QStringList& _previews)
{
    db_op->GetModelInfo(_loc_num, _title_model, _description, _files_model, _previews);
}
//----------------------------------------------------------
QString MainModule::LoginFromNum(const qlonglong _num_login) const
{
    return db_op->LoginFromNum(_num_login);
}
//----------------------------------------------------------
QString MainModule::GetServerModelPath(const qlonglong _num_server) const
{
    return db_op->GetServerModelPath(_num_server);
}
//----------------------------------------------------------
void MainModule::GetServerModelInfo(const qlonglong _serv_num, QString& _title_model, QString &_description, QList<tFile> &_files_model)
{
    db_op->GetServerModelInfo(_serv_num, _title_model, _description, _files_model);
}
//----------------------------------------------------------
void MainModule::SaveDescription(const qlonglong _num, const QString &_text, QString &_file_name)
{
    db_op->SaveDescription(_num, _text, _file_name);
}
//----------------------------------------------------------
void MainModule::UpdateInfoData(const qlonglong _num_model, const QString &_file_name, const QString &_hash, const QDateTime &_last_mod)
{
    db_op->UpdateInfoData(_num_model, _file_name, _hash, _last_mod);
}
//----------------------------------------------------------
QString MainModule::GetRoot()
{
    return my_settings.GetRoot();
}
//----------------------------------------------------------
void MainModule::WriteToCompareTablesToTree(const QString& _login)
{
    db_op->WriteToCompareTablesToTree(_login);
}
//----------------------------------------------------------
QList<CompareTableRec> MainModule::AnalyzeCompareAll()
{
    return db_op->AnalyzeCompareAll();
}
//----------------------------------------------------------
void MainModule::AddFilesToModelsStruct(QList<CompareTableRec> &comp_models)
{
    db_op->AddFilesToModelsStruct(comp_models);
}
//----------------------------------------------------------
void MainModule::SetTransactionFlag(const bool _flag)
{
    mod_conv->SetTransactionFlag(_flag);
}
//----------------------------------------------------------
void MainModule::ClearConveyor()
{
    mod_conv->Clear();
}
//----------------------------------------------------------
void MainModule::DeletingLocalFile(const QString& _file_name)
{
    mod_conv->DeletingLocalFile(_file_name);
}
//----------------------------------------------------------
void MainModule::ReceiveFile(const QString& _file_name)
{
    mod_conv->ReceiveFile(_file_name);
}
//----------------------------------------------------------
void MainModule::GetServerListPreviews(const qlonglong _server_num_model, QStringList &_list)
{
    db_op->GetServerListPreviews(_server_num_model, _list);
}
//----------------------------------------------------------
void MainModule::GetServerListFiles(const qlonglong _num_server_model, QStringList &_list)
{
    db_op->GetServerListFiles(_num_server_model, _list);
}
//----------------------------------------------------------
int MainModule::GetCountRecDelModels()
{
    return db_op->GetCountRecDelModels();
}
//----------------------------------------------------------
void MainModule::StartReceiveDeleteFiles(const QString &_root, int _custom_copy, int max_model)
{
    mod_conv->StartReceiveDeleteFiles(_root, _custom_copy, max_model);
}
//----------------------------------------------------------
void MainModule::setValue(int _value)
{
    emit SignalCountFiles(_value);
}
//----------------------------------------------------------
void MainModule::OnEndTransactions()
{
    log.Write(tr("MainModule \t OnEndTransactions \t КОНЕЦ ТРАНЗАКЦИЙ"));
    qDebug() << "КОНЕЦ ТРАНЗАКЦИЙ";
    emit EndTransactions();
}
//----------------------------------------------------------
void MainModule::DeletingServerFile(const QString &_file_name)
{
    mod_conv->DeletingServerFile(_file_name);
}
//----------------------------------------------------------
void MainModule::SendFile(const QString &_file_name)
{
    mod_conv->SendFile(_file_name);
}
//----------------------------------------------------------
int MainModule::GetCountSendDelModels()
{
    return db_op->GetCountSendDelModels();
}
//----------------------------------------------------------
void MainModule::StartSendDeleteFiles(const int _max_model)
{
    mod_conv->StartSendDeleteFiles(_max_model);
}
//----------------------------------------------------------
void MainModule::ActualiseModel(const QString &_login, const qlonglong _num_model, const bool _from_server)
{
    db_op->ActualiseModel(_login, _num_model, _from_server);
}
//----------------------------------------------------------
QString MainModule::VerifyCustomCopyPath(const QString& path) const
{
    return db_op->VerifyCustomCopyPath(path);
}
//----------------------------------------------------------
void MainModule::OnRunGui(QByteArray& _block)
{
    emit RunGui(_block);
}
//----------------------------------------------------------
void MainModule::OnDisconnectingFromClient()
{
    emit Disconnecting();
}
//----------------------------------------------------------
void MainModule::SetServerParameters(const QString &_addr, const int _port)
{
    my_settings.SetServerAddr(_addr);
    my_settings.SetServerPort(_port);
    my_settings.sync();
}
//----------------------------------------------------------
void MainModule::StartServer()
{
    mod_conv->StartServer(my_settings.GetServerAddr(), my_settings.GetServerPort());
}
//----------------------------------------------------------
void MainModule::GetServerParameters(QString &_addr, int& _port)
{
    _addr=my_settings.GetServerAddr();
    _port=my_settings.GetServerPort();
}
//----------------------------------------------------------
void MainModule::GetFolderParameters(const QString& login, QString& roor_folder, QString& temp_folder, QString& mess)
{
    db_op->VerifyUserFolders(login, roor_folder, temp_folder, mess);
    qDebug() << "MainModule::GetFolderParameters" << roor_folder << temp_folder << mess;
}
//----------------------------------------------------------
void MainModule::SaveFoldersToLoginsTable(const QString &_login, const QString &_pr_folder, const QString &_temp_folder)
{
    db_op->SaveFoldersToLoginsTable(_login, _pr_folder, _temp_folder);
}
//----------------------------------------------------------
void MainModule::SaveFoldersToSettings(const QString& _user_login)
{
    db_op->SaveFoldersToSettings(_user_login);
}
//----------------------------------------------------------
void MainModule::GetPermissionsUser(const QString& _login, bool& _is_admin_user, bool& _is_writable_user)
{
    db_op->GetPermissionsUser(_login, _is_admin_user, _is_writable_user);
}
//----------------------------------------------------------
void MainModule::RefreshModelsFiles()
{
    db_op->RefreshModelsFiles();
}
//----------------------------------------------------------
bool MainModule::removeFolder(const QDir & _dir, const bool _del_dir)
{
    bool res = false;
    //Получаем список каталогов
    QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                                          QDir::AllDirs |
                                          QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

    //Удаляем файлы
    foreach (QString entry, lst_files)
    {
        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QFile::setPermissions(entry_abs_path, QFile::ReadOwner | QFile::WriteOwner);
        QFile::remove(entry_abs_path);
    }

    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lst_dirs)
    {
        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QDir dr(entry_abs_path);
        removeFolder(dr, true);
    }

    //Удаляем обрабатываемую папку
    if(_del_dir)
    {
        SetFileAttributes((LPCWSTR)(_dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
        QFile::setPermissions(_dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
        if (!QDir().rmdir(_dir.absolutePath()))
        {
            res = true;
        }
        else
        {
            res=false;
        }
    }
    else
    {
        res = true;
    }
    return res;
}
//----------------------------------------------------------
void MainModule::SendDeleteLogin(int _num)
{
    mod_conv->SendDeleteLogin(_num);
}
//----------------------------------------------------------
void MainModule::SaveLoginWritable(const QStandardItemModel *_model, const int _row)
{
    mod_conv->SaveLoginWritable(_model, _row);
}
//----------------------------------------------------------
void MainModule::GetLoginsModel(QStandardItemModel *model)
{
    db_op->GetLoginsModel(model);
}
//----------------------------------------------------------
bool MainModule::VerPassword(const QString &login, const QString &_pass)
{
    return db_op->VerPassword(login, _pass);
}
//----------------------------------------------------------
void MainModule::SavePermissionsToServer(const qlonglong _num_login)
{
    mod_conv->SavePermissionsToServer(_num_login);
}
//----------------------------------------------------------
qlonglong MainModule::GetNumLogin(const int _row) const
{
    return db_op->GetNumLogin(_row);
}
//----------------------------------------------------------
qlonglong MainModule::GetNumLogin(const QString &_login) const
{
    return db_op->GetNumLogin(_login);
}
//----------------------------------------------------------
void MainModule::SaveReadPermission(const QString &_login, const qlonglong _mod_num, const bool _state)
{
    db_op->SaveReadPermission(_login, _mod_num, _state);
}
//----------------------------------------------------------
void MainModule::ResetFoundModelAdmin()
{
    db_op->ResetFoundModelAdmin();
}
//----------------------------------------------------------
bool MainModule::NextModelAdmin() const
{
    return db_op->NextModelAdmin();
}
//----------------------------------------------------------
QStringList MainModule::NextStructListModelAdmin(const QString &_login, bool &_read, qlonglong &_server_num) const
{
    return db_op->NextStructListModelAdmin(_login, _read, _server_num);
}
//----------------------------------------------------------
void MainModule::SendLoginPassword(const QString &_login, const QString &_password, const int _row, const bool _new_user)
{
    login=_login;
    password=_password;
    new_user=_new_user;
    mod_conv->SendLoginPassword(_login, _password, _row, _new_user);
}
