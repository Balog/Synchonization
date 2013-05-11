#include "MainForm.h"
#include "ui_MainForm.h"
#include <QFileDialog>
#include "tLog.h"
#include <QRegExp>


extern tSettings my_settings;

MainForm::MainForm(QWidget *parent) : ui(new Ui::MainForm), QDialog(parent),mod_conv(NULL), db_op(NULL), sLM_loc_list_models(NULL), slm_server_list_models(NULL), slm_list(NULL), login_pass(new tEditLoginPass)
{


    sLM_Send=new QStringListModel;
    sLM_Del=new QStringListModel;
    sLM_Rec=new QStringListModel;
    sLM_DelLoc=new QStringListModel;
    sLM_Logins=new QStringListModel;

    ui->setupUi(this);

    //    connect(ui->lvListFiles, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnClickListFiles(QModelIndex)));

    ui->leRoot->setText(my_settings.GetRoot());
    ui->leTemp->setText(my_settings.GetTemp());
    ui->leAddr->setText(my_settings.GetServerAddr());
    ui->sbPort->setValue(my_settings.GetServerPort());




    my_settings.SetRoot(ui->leRoot->text());
    my_settings.SetTemp(ui->leTemp->text());
    my_settings.SetServerAddr(ui->leAddr->text());
    my_settings.SetServerPort(ui->sbPort->value());
    my_settings.sync();

    db_op=new tDatabaseOp();

    db_op->RefreshModelsFiles();

    mod_conv= new tModelsConveyor(ui, this, db_op);

    mod_conv->StartServer(ui->leAddr->text(), ui->sbPort->value());

    login_pass->setVisible(false);
    connect(login_pass, SIGNAL(EndEditing(QString&,QString&,int,bool)), this, SLOT(OnEndEditLoginPassword(QString&,QString&,int,bool)));

    UpdateLogins();

    tLog log;

    log.Write(tr("Начало работы клиента"));
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete sLM_loc_list_models;
    delete slm_list;

    delete sLM_Send;
    delete sLM_Del;
    delete sLM_Rec;
    delete sLM_DelLoc;
    delete sLM_Logins;

    delete mod_conv;
    mod_conv=NULL;

    delete db_op;
    db_op=NULL;

    delete ui;

    tLog log;
    log.Write(tr("Конец работы клиента"));
}
//---------------------------------------------------------------------
void MainForm::Autorization(QString& _login, QString& _password)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SendAutorization");
    out << tr("SendAutorization");
    out << _login;
    out << _password;

    emit RunGui(block);
}
//---------------------------------------------------------------------
void MainForm::OnConnect()
{
    ui->pbConnect->setEnabled(false);
    ui->pbDisconnect->setEnabled(true);

    my_settings.SetServerAddr(ui->leAddr->text());
    my_settings.SetServerPort(ui->sbPort->value());
    my_settings.SetRoot(ui->leRoot->text());
    my_settings.SetTemp(ui->leTemp->text());
    my_settings.sync();

    this->setVisible(false);

    QString S=my_settings.GetServerAddr();
    mod_conv->StartServer(S, my_settings.GetServerPort());

    emit FindServer();
}
//---------------------------------------------------------------------
void MainForm::OnDisconnect()
{
    ui->pbConnect->setEnabled(true);
    ui->pbDisconnect->setEnabled(false);

    mod_conv->Clear();

}
//---------------------------------------------------------------------
void MainForm::EndTransactions()
{
    mod_conv->SetTransactionFlag(false);

    tLog log;
    log.Write(tr("Пакет транзакций выполнен"));

    QMessageBox MB;
    MB.setText(QString::fromUtf8("Пакет транзакций выполнен"));
    MB.setWindowTitle(QString::fromUtf8("Пакет транзакций"));
    MB.exec();
}
//---------------------------------------------------------------------
void MainForm::OnAutorizStart()
{
    emit AutorizStart();
}
//---------------------------------------------------------------------
void MainForm::OnSetVisible(const bool vis)
{
    this->setVisible(vis);
}
//---------------------------------------------------------------------
void MainForm::OnDisconnecting()
{
    emit Disconnecting();
}
//---------------------------------------------------------------------
void MainForm::OnAddSend()
{
    QModelIndex MI=ui->lvLocalListFiles->currentIndex();

    int N=MI.row();
    if(N<0)
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Выделите клиентскую модель"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
    else
    {
        QStringListModel *M=new QStringListModel;
        M=(QStringListModel *)MI.model();

        QString S=M->stringList().value(N);

        listSend.push_back(S);
        sLM_Send->setStringList(listSend);
        ui->lvSendingFiles->setModel(sLM_Send);
    }
}
//---------------------------------------------------------------------
void MainForm::OnStartSend()
{
    mod_conv->SetTransactionFlag(true);
    mod_conv->Clear();
    if(listSend.size()!=0)
    {
        for(int i=0; i<listSend.size(); i++)
        {
            QString S=listSend[i];
            mod_conv->SendFile(S);
        }
    }

    if(listDel.size()!=0)
    {
        for(int i=0; i<listDel.size(); i++)
        {
            QString S=listDel[i];
            mod_conv->DeletingServerFile(S);
        }

    }

    mod_conv->StartSendDeleteFiles();

}
//---------------------------------------------------------------------
void MainForm::OnClearSendAll()
{
    listSend.clear();
    sLM_Send->setStringList(listSend);
}
//---------------------------------------------------------------------
void MainForm::OnAddDelete()
{
    QModelIndex MI=ui->lvListFiles->currentIndex();

    int N=MI.row();
    QStringListModel *M=new QStringListModel;
    M=(QStringListModel *)MI.model();

    QString S=M->stringList().value(N);

    listDel.push_back(S);
    sLM_Del->setStringList(listDel);
    ui->lvDeletingFiles->setModel(sLM_Del);
}
//---------------------------------------------------------------------
void MainForm::OnClearDelete()
{
    listDel.clear();
    sLM_Del->setStringList(listDel);
}
//---------------------------------------------------------------------
void MainForm::OnListFiles()
{

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GetListModels");
    out << tr("GetListModels");

    emit RunGui(block);
}
//---------------------------------------------------------------------
void MainForm::OnAddReceive()
{
    QModelIndex MI=ui->lvListFiles->currentIndex();

    int N=MI.row();
    if(N<0)
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Выделите серверную модель"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
    else
    {
        QStringListModel *M=new QStringListModel;
        M=(QStringListModel *)MI.model();

        QString S=M->stringList().value(N);

        listRec.push_back(S);
        sLM_Rec->setStringList(listRec);
        ui->lvReceiveFiles->setModel(sLM_Rec);
    }
}
//---------------------------------------------------------------------
void MainForm::OnReceiveClear()
{
    listRec.clear();
    sLM_Rec->setStringList(listRec);
}
//---------------------------------------------------------------------
void MainForm::OnDeleteLocal()
{
    QModelIndex MI=ui->lvLocalListFiles->currentIndex();

    int N=MI.row();
    QStringListModel *M=new QStringListModel;
    M=(QStringListModel *)MI.model();

    QString S=M->stringList().value(N);

    listDelLoc.push_back(S);
    sLM_DelLoc->setStringList(listDelLoc);
    ui->lvLocalDeletingFiles->setModel(sLM_DelLoc);
}
//---------------------------------------------------------------------
void MainForm::OnClearDeleteLocal()
{
    listDelLoc.clear();
    sLM_DelLoc->setStringList(listDelLoc);

}
//---------------------------------------------------------------------
void MainForm::OnStartReceive()
{
    mod_conv->SetTransactionFlag(true);
    mod_conv->Clear();
    if(listRec.size()>0)
    {
        for(int i=0; i<listRec.size(); i++)
        {
            QString S=listRec[i];
            mod_conv->ReceiveFile(S);
        }
    }

    if(listDelLoc.size()>0)
    {
        for(int i=0;i<listDelLoc.size(); i++)
        {
            QString S=listDelLoc[i];
            mod_conv->DeletingLocalFile(S);
        }
    }

    mod_conv->StartReceiveDeleteFiles();


}
//---------------------------------------------------------------------
void MainForm::OnListFilesLocal()
{
    QString root=my_settings.GetRoot();
    QStringList list;
    db_op->RefreshModelsFiles();
    SearchModelsOnDatabase(list);

    sLM_loc_list_models=new QStringListModel;
    sLM_loc_list_models->setStringList(list);
    ui->lwLocalListModels->setModel(sLM_loc_list_models);
}
//---------------------------------------------------------------------
void MainForm::CreateFileList(const QString &start_folder, QStringList& list)
{
    QDir dir(start_folder);
    SearchFiles(dir, list);
}
//----------------------------------------------------------
void MainForm::SearchFiles(const QDir & dir, QStringList &list)
{
    QStringList lstDirs = dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lstFiles = dir.entryList(QDir::Files | QDir::Hidden);

    //записываем файлы
    foreach (QString entry, lstFiles)
    {
        QString entryAbsPath = dir.absolutePath() + "/" + entry;
        list.push_back(entryAbsPath);
    }

    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lstDirs)
    {
        QString entryAbsPath = dir.absolutePath() + "/" + entry;
        QDir dr(entryAbsPath);
        SearchFiles(dr, list);
    }
}
//----------------------------------------------------------
void MainForm::SearchModelsOnDatabase(QStringList &_list)
{
    db_op->GetListModels(_list);
}
//----------------------------------------------------------
void MainForm::OnUpdateStruct()
{
    //    db_op->RefreshModelsFiles();
}
//----------------------------------------------------------
void MainForm::LocalListFile(const QStringList &list)
{
    slm_list=new QStringListModel;
    slm_list->setStringList(list);
    ui->lvListFiles->setModel(slm_list);
}
//----------------------------------------------------------
void MainForm::CancelAllOperations()
{
    OnClearSendAll();
    OnClearDelete();
    OnReceiveClear();
    OnClearDeleteLocal();

    mod_conv->CancelOperations();
}
//----------------------------------------------------------
void MainForm::OnClearModels()
{
    db_op->ClearModels();
}
//----------------------------------------------------------
void MainForm::OnLocalModelClick(const QModelIndex Ind)
{
    //выбор локальной модели для отображения файлов
    int N=Ind.row();
    QString str=sLM_loc_list_models->stringList().at(N);

    QStringList list;
    db_op->GetLocalModelFiles(str, list);

    sLM_loc_list_files=new QStringListModel;
    sLM_loc_list_files->setStringList(list);
    ui->lvLocalListFiles->setModel(sLM_loc_list_files);
}
//----------------------------------------------------------
void MainForm::SaveServerModelFiles(QByteArray &_block)
{
    //распарсить переданый блок и записать в папки серверного состояния моделей и их файлов
    db_op->SaveServerModelFiles(_block);
    QStringList list;
    //Получить из папки серверного состояния моделей список моделей
    db_op->GetServerListModels(list);

    //отобразить список
    slm_server_list_models=new QStringListModel;
    slm_server_list_models->setStringList(list);
    ui->lwListModels->setModel(slm_server_list_models);
}
//----------------------------------------------------------
void MainForm::OnServerModelClick(const QModelIndex Ind)
{
    //выбор серверной модели для отображения файлов
    int N=Ind.row();
    QString str=slm_server_list_models->stringList().at(N);

    QStringList list;

    db_op->GetServerListFiles(str, list);

    sLM_server_list_files=new QStringListModel;
    sLM_server_list_files->setStringList(list);
    ui->lvListFiles->setModel(sLM_server_list_files);
}
//----------------------------------------------------------
void MainForm::CorrectLastSynch(bool _server)
{

    mod_conv->MarkLastTables(_server);

    db_op->ExecUpdateLastSynch(_server);

}
//----------------------------------------------------------
void MainForm::OnNewLogin()
{
    login_pass->setModal(true);
    login_pass->setVisible(true);
    login_pass->new_user=true;
    login_pass->row=-1;


}
//----------------------------------------------------------
void MainForm::OnEditLogin()
{
    QModelIndex MI=ui->lvLogins->currentIndex();
    int N=MI.row();
    if(N>=0)
    {
    QStringListModel *sLM_Logins=new QStringListModel;
    sLM_Logins=(QStringListModel *)MI.model();
    QString S=sLM_Logins->stringList().value(N);
//    delete M;

    login_pass->setModal(true);
    login_pass->setVisible(true);
    login_pass->SetLogin(S);
    login_pass->new_user=false;
    login_pass->row=N;

    mi=ui->lvLogins->selectionModel()->currentIndex();
    }
    else
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Выделите редактируемый логин"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
}
//----------------------------------------------------------
void MainForm::OnDelLogin()
{
    QModelIndex MI=ui->lvLogins->currentIndex();
    int N=MI.row();
    if(N>=0)
    {
    mod_conv->SendDeleteLogin(N);
    }
    else
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Выделите удаляемый логин"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
}
//----------------------------------------------------------
void MainForm::OnEndEditLoginPassword(QString& _login, QString& _password, int _row, bool _new_user)
{
    if(_login.length()>0 && _password.length()>0)
    {
        QRegExp exp;
        exp.setPattern("^[a-zA-Z0-9]+$");

        if(exp.indexIn(_login)>=0)
        {
            //Логин и пароль проверены на клиенте и отправляются на сервер для дальнейшей проверки в БД и регистрации

            mod_conv->SendLoginPassword(_login, _password, _row, _new_user);
//            login_pass->setModal(false);
//            login_pass->setVisible(false);


        }
        else
        {
            QMessageBox MB;
            MB.setText(QString::fromUtf8("Логин содержит недопустимые символы"));
            MB.setWindowTitle(QString::fromUtf8("Ошибка"));
            MB.exec();
        }
    }
    else
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Логин и пароль не могут быть пустыми"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
}
//----------------------------------------------------------
void MainForm::OnLoginsClicked(QModelIndex Ind)
{
    int N=Ind.row();
    if(N>=0)
    {
        ui->pbEditUser->setEnabled(true);
        ui->pbDelUser->setEnabled(true);
    }
    else
    {
        ui->pbEditUser->setEnabled(false);
        ui->pbDelUser->setEnabled(false);
    }
}
//----------------------------------------------------------
void MainForm::RegisterUser(qlonglong _s_num)
{
    QString login=login_pass->GetLogin();
    QString password=login_pass->GetPassword();
    bool new_user=login_pass->new_user;
    db_op->SaveLoginPassword(login, password, new_user, _s_num);

    login_pass->setModal(false);
    login_pass->setVisible(false);
    login_pass->ClearAll();

    UpdateLogins();
}
//----------------------------------------------------------
void MainForm::UpdateLogins()
{
    listLogins=db_op->GetLoginsList();
    sLM_Logins->setStringList(listLogins);
    ui->lvLogins->setModel(sLM_Logins);

    ui->lvLogins->selectionModel()->setCurrentIndex(mi, QItemSelectionModel::Select);
}
//----------------------------------------------------------
void MainForm::DeleteUser(qlonglong _s_num)
{
    db_op->DeleteLogin(_s_num);

    login_pass->setModal(false);
    login_pass->setVisible(false);
    login_pass->ClearAll();

    UpdateLogins();
}
//----------------------------------------------------------
void MainForm::OnReceiveLoginsTable()
{
    mod_conv->ReceiveLoginsTable();
}
//----------------------------------------------------------
void MainForm::UpfateLoginsTable(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateLogins(_block);

    UpdateLogins();

    emit StartAutorizForm();
//    QStringList list;
//    //Получить из папки серверного состояния моделей список моделей
//    db_op->GetServerListModels(list);

//    //отобразить список
//    slm_server_list_models=new QStringListModel;
//    slm_server_list_models->setStringList(list);
//    ui->lwListModels->setModel(slm_server_list_models);
}
//----------------------------------------------------------
void MainForm::OnVerPassword()
{
    if(db_op->VerPassword(ui->lvLogins->currentIndex().data().toString(), ui->le_ver_pass->text()))
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Пароль указан верно"));
        MB.setWindowTitle(QString::fromUtf8("Проверка пароля"));
        MB.exec();
    }
    else
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Пароль указан неверно"));
        MB.setWindowTitle(QString::fromUtf8("Проверка пароля"));
        MB.exec();
    }
}
