#include "MainForm.h"
#include "ui_MainForm.h"
#include <QFileDialog>
#include "tLog.h"
#include <QRegExp>
#include <QMenu>


extern tSettings my_settings;

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent),mod_conv(NULL), db_op(NULL),
    sLM_loc_list_models(NULL), slm_server_list_models(NULL), slm_list(NULL),
    login_pass(new tEditLoginPass), adm_tree_model(NULL), form_new_path(new tNewPath),
    read_tree_model(new QStandardItemModel()), write_tree_model(new QStandardItemModel())
{
//    constr_mod_tree=NULL;
    IsRequeryServerModel=false;
    user_login="";

    sLM_Send=new QStringListModel;
    sLM_Del=new QStringListModel;
    sLM_Rec=new QStringListModel;
    sLM_DelLoc=new QStringListModel;
    sLM_Logins=new QStringListModel;

    adm_tree_model= new QStandardItemModel();;

    ui->setupUi(this);

    //    connect(ui->lvListFiles, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnClickListFiles(QModelIndex)));

//    ui->leRoot->setText(my_settings.GetRoot());
//    ui->leTemp->setText(my_settings.GetTemp());
    ui->leAddr->setText(my_settings.GetServerAddr());
    ui->sbPort->setValue(my_settings.GetServerPort());




//    my_settings.SetRoot(ui->leRoot->text());
//    my_settings.SetTemp(ui->leTemp->text());
    my_settings.SetServerAddr(ui->leAddr->text());
    my_settings.SetServerPort(ui->sbPort->value());
    my_settings.sync();

    db_op=new tDatabaseOp();



    mod_conv= new tModelsConveyor(ui, this, db_op);

    mod_conv->StartServer(ui->leAddr->text(), ui->sbPort->value());

    login_pass->setVisible(false);
    connect(login_pass, SIGNAL(EndEditing(QString&,QString&,int,bool)), this, SLOT(OnEndEditLoginPassword(QString&,QString&,int,bool)));
    connect(form_new_path, SIGNAL(ContinueStrat()), this, SLOT(OnContinueStart()));

//    UpdateLogins();

    ui->tvRead->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tvRead, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(showContextMenuRead(QPoint)));


    tLog log;

    log.Write(tr("Начало работы клиента"));
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete form_new_path;
    delete login_pass;

    delete sLM_loc_list_models;
    delete slm_list;

    delete sLM_Send;
    delete sLM_Del;
    delete sLM_Rec;
    delete sLM_DelLoc;
    delete sLM_Logins;
    delete slm_server_list_models;
    delete adm_tree_model;
    delete read_tree_model;
    delete write_tree_model;

    delete mod_conv;
    mod_conv=NULL;

    delete db_op;
    db_op=NULL;



    delete ui;

    tLog log;
    log.Write(tr("Конец работы клиента"));
}
//---------------------------------------------------------------------
void MainForm::Autorization(QString& _login, QString& _password, bool _modify_folder)
{
    user_login=_login;
    this->setWindowTitle(QString::fromUtf8(QString("Главная форма (Пользователь '"+user_login+"')").toAscii()));
    modify_folder=_modify_folder;
    mod_conv->SetLogin(user_login);
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
    adm_tree_model->clear();
    ui->pbConnect->setEnabled(false);
    ui->pbDisconnect->setEnabled(true);
    ui->leAddr->setReadOnly(true);
    ui->sbPort->setReadOnly(true);

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
    ui->leAddr->setReadOnly(false);
    ui->sbPort->setReadOnly(false);

    mod_conv->Clear();

}
//---------------------------------------------------------------------
void MainForm::EndTransactions()
{
    mod_conv->SetTransactionFlag(false);

    IsRequeryServerModel=true;
    OnListFiles();

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
    tLog log;
    log.Write(tr("MainForm \t OnListFiles \t Запрос с главной формы GetListModels через RunGui"));

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
//    QString root=my_settings.GetRoot();
    QStringList list;
    db_op->RefreshModelsFiles();
    SearchModelsOnDatabase(list);

    VerifyLastTable(user_login);

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

    mod_conv->MarkLastTables(_server, user_login);

    db_op->ExecUpdateLastSynch(_server, user_login);
    mod_conv->ClearAllList();
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
void MainForm::UpdateLoginsTable(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateLogins(_block);

    UpdateLogins();


//    OnListFilesLocal();


    StartAutoriz();



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
//----------------------------------------------------------
void MainForm::TreesBuildings(QString& _login)
{
    //Постороение деревьев моделей
    tLog log;
    log.Write(tr("Постороение деревьев моделей"));

    adm_tree_model->clear();
    tConstructModelTree *constr_mod_tree=new tConstructModelTree(db_op,  _login);


    while(constr_mod_tree->NextModelAdmin())
    {
        bool read=false;
        qlonglong server_num_model=-1;

        QStringList list_model=constr_mod_tree->ListAdmin(read, server_num_model);

        //добавить модель к дереву

        //получить корень дерева

        QStandardItem *item=adm_tree_model->invisibleRootItem();
        item->setCheckable(true);
        //        item=item->child(0);
        for(int i=0; i<list_model.size(); i++)
        {
            QString txt=item->text();
//            log.Write(QString(QString("Ветви уже есть. Значение ")+txt.toUtf8()));
            //ветви уже есть
            //проверить всех потомков текущей ветви
            //если есть совпадение то перейти к этому потомку
            //если нет - создать нового потомка
            bool find=false;
            int row_count=item->rowCount();
            for(int j=0; j<row_count; j++)
            {
                QString ch_text=item->child(j)->text();
                QString mod_text=list_model[i];
//                log.Write(QString(QString("Сверяю потомков. Ветвь: ")+ch_text.toUtf8()+QString(" Модель: ")+mod_text.toUtf8()));

                if(ch_text==mod_text)
                {
//                    log.Write(QString("Потомок найден "));
                    Qt::CheckState st=item->child(j)->checkState();

                    switch (st)
                    {
                    case Qt::Checked:
                    {
//                        log.Write(QString("прошлое состояние - включено "));
                        if(!read)
                        {
                            item->child(j)->setCheckState(Qt::PartiallyChecked);
//                            log.Write(QString("Установлено - среднее "));
                        }
                        else
                        {
                            item->child(j)->setCheckState(Qt::Checked);
//                            log.Write(QString("Установлено - включено "));
                        }
                        break;
                    }
                    case Qt::Unchecked:
                    {
//                        log.Write(QString("прошлое состояние - выключено "));
                        if(read)
                        {
                            item->child(j)->setCheckState(Qt::PartiallyChecked);
//                            log.Write(QString("Установлено - среднее "));
                        }
                        else
                        {
                            item->child(j)->setCheckState(Qt::Unchecked);
//                            log.Write(QString("Установлено - выключено "));
                        }
                        break;
                    }
                    case Qt::PartiallyChecked:
                    {
//                        log.Write(QString("прошлое состояние - среднее "));
                        break;
                    }
                    }

                    item=item->child(j);
                    QString t=item->text();
                    find=true;
                    break;
                }
            }
            if(!find)
            {
                QStandardItem *new_item=new QStandardItem(list_model[i]);
//                bool is_model=i==list_model.size()-1;
                QFont font;
                if(i==list_model.size()-1)
                {
                    new_item->setData(server_num_model);
                    font.setBold(true);
                }
                else
                {
                    new_item->setData(-1);
                    font.setBold(false);
                }
//                new_item->setData((QVariant)data);
                new_item->setFont(font);
                new_item->setCheckable(true);
                new_item->setTristate(true);
                if(read)
                {
                new_item->setCheckState(Qt::Checked);
//                log.Write(QString("Новая ветвь. Установлено - включено "+new_item->text().toUtf8()));
                }
                else
                {
                new_item->setCheckState(Qt::Unchecked);
//                log.Write(QString("Новая ветвь. Установлено - выключено "+new_item->text().toUtf8()));
                }
                new_item->setEditable(false);
                new_item->setSelectable(false);
//                QIcon icon(":/Icons/Tree/Del_sm.ico");
//                new_item->setIcon(icon);

                item->appendRow(new_item);


                Qt::CheckState new_check;
                if(item->checkState()==Qt::PartiallyChecked)
                {
                if(read)
                {
                    new_check=Qt::Unchecked;
                }
                else
                {
                    new_check=Qt::Checked;
                }

                UpToParent(new_item->index(), new_check);
                }
                item=new_item;
            }
        }
    }

    ui->tvAdminTree->setModel(adm_tree_model);
    ui->tvAdminTree->setHeaderHidden(true);
    ui->tvAdminTree->expandAll();
    ui->tvAdminTree->setRootIsDecorated(true);
    ui->tvAdminTree->setAnimated(true);
    delete constr_mod_tree;
    constr_mod_tree=NULL;
}
//----------------------------------------------------------
void MainForm::StartAutoriz()
{
    //показ формы авторизации
    tLog log;
    log.Write(tr("показ формы авторизации"));
    emit StartAutorizForm();
}
//----------------------------------------------------------
void MainForm::on_tvAdminTree_clicked(const QModelIndex &index)
{
    //Для модификации отметок нужно проделать две операции:
    //1. Вверх по предкам
    //1.1. От предка пройти по всем его потомкам (в том числе и текущей ветви) и проверить их состояние
    //     Если однозначное то такое же выставить предку, если неоднозначное поставить неопределенное.
    //     Определенное и неопределенное в сумме дает неопределенное
    //1.2. Перейти к предку предка и повторять операцию до самого корня

    //2. Вниз по потомкам
    //2.1. если текущая ветвь выставлена в истину то выставить всех потомков в истину
    //     если текущая ветвь выставлена в ложь то всех потомков выставить в ложь

    //В конце нужно будет сохранить состояние в таблице и синхронизировать на сервер


    UpToParent(index, adm_tree_model->itemFromIndex(index)->checkState());
    DownToChildrens(index, adm_tree_model->itemFromIndex(index)->checkState());
    mod_conv->SavePermissionsToServer(db_op->GetNumLogin(sLM_Logins->stringList().value(admin_logins_index.row())));
}
//----------------------------------------------------------
void MainForm::UpdateModelRead(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateModelRead(_block);

    this->setVisible(true);

//    StartAutoriz();

}
//----------------------------------------------------------

void MainForm::on_lvLogins_clicked(const QModelIndex &index)
{
    admin_logins_index=index;
    QString S=sLM_Logins->stringList().value(index.row());

    TreesBuildings(S);

}
//----------------------------------------------------------
void MainForm::UpToParent(QModelIndex index, Qt::CheckState _state)
{

    //определим предка
    const QModelIndex parent_index=adm_tree_model->parent(index);
    QStandardItem *parent_item=adm_tree_model->itemFromIndex(parent_index);
    if(!parent_item)
    {
        return;
    }
    else
    {


    int child_count=parent_item->rowCount();
    Qt::CheckState state=_state;
    if(child_count>1)
    {
    for(int i=0; i<child_count;i++)
    {
        QStandardItem *child_item=parent_item->child(i);
        switch (child_item->checkState())
        {
        case Qt::Checked:
        {
            if(state!=Qt::Checked)
            {
                state=Qt::PartiallyChecked;
            }
            break;
        }
        case Qt::Unchecked:
        {
            if(state!=Qt::Unchecked)
            {
                state=Qt::PartiallyChecked;
            }
            break;
        }
        case Qt::PartiallyChecked:
        {
            state=Qt::PartiallyChecked;
            break;
        }
        }
        if(state==Qt::PartiallyChecked)
        {
            break;
        }
    }
    }
    else
    {
        qlonglong s_num=parent_item->data().toLongLong();
        if(s_num>0)
        {
            state=Qt::Checked;

        }
    }
    //установим полученый state предку
    parent_item->setCheckState(state);

    UpToParent(parent_index, state);
    }
}
//----------------------------------------------------------
void MainForm::DownToChildrens(QModelIndex index, Qt::CheckState _state)
{
    QStandardItem *item=adm_tree_model->itemFromIndex(index);

    qlonglong s_num=item->data().toLongLong();
    if(s_num>0)
    {
        //это модель и s_num ее серверный номер
        //обновить таблицу ModelRead
        bool state=false;
        if(_state==Qt::Checked)
        {
            state=true;
        }
        db_op->SaveReadPermission(sLM_Logins->stringList().value(admin_logins_index.row()), s_num, state);
    }

    int child_count=item->rowCount();
    if(child_count==0)
    {
        return;
    }
    else
    {
        for(int i=0; i<child_count; i++)
        {
            QStandardItem *children=item->child(i);
            children->setCheckState(_state);
            QModelIndex child_index=children->index();


            DownToChildrens(child_index, _state);
        }
    }
}
//----------------------------------------------------------

void MainForm::on_pbListFiles_clicked()
{
    //старая кнопка
    IsRequeryServerModel=true;
    OnListFiles();
}
//----------------------------------------------------------
bool MainForm::VerifyUserFolders()
{
    QString message="";
    QString project_folder="";
    QString temp_folder="";
    if(!modify_folder && db_op->VerifyUserFolders(user_login, project_folder, temp_folder, message))
    {

        return true;
    }
    else
    {
        form_new_path->SetDatabase(db_op);
        form_new_path->SetLogin(user_login);
        form_new_path->SetMessage(message);
        form_new_path->setVisible(true);
        return false;
    }

}
//----------------------------------------------------------
void MainForm::OnContinueStart()
{
    db_op->SaveFoldersToSettings(user_login);
    ui->leRoot->setText(my_settings.GetRoot());
    ui->leTemp->setText(my_settings.GetTemp());

db_op->RefreshModelsFiles();

OnListFilesLocal();
    OnListFiles();

}
//----------------------------------------------------------
void MainForm::VerifyLastTable(const QString& user_login)
{
    //Проверить если есть такие модели в таблице Last
    //которых уже нет ни в серверной ни в локальной папке
    db_op->VerifyLastTable(user_login);
    BuildingTree(user_login, Read);
    BuildingTree(user_login, Write);
}
//----------------------------------------------------------
void MainForm::on_pbExit_clicked()
{
    this->close();
}
//----------------------------------------------------------


void MainForm::on_pbRead_clicked()
{

//    struct tFile
//    {
//        bool Local; // 1 - локальный файл, 0 - серверный файл
//        qlonglong num;
//        QString file_name;
//        qint64 size;
//        QString last_mod;
//        int IsFounded;  //флаг IsFounded (1 - есть только на локальном, 2 - есть только на серверном, 0 - есть и там и там)

//    } ;

    //начать чтение с сервера по новому
////сначала составим списки запрашиваемых с сервера и удаляемых локально файлов
//QList<QString>rec_files;
//QList<QString>loc_del_files;

//если запрашиваемый файл есть только локально а на сервере его нет то это удаление файла локально
//остальные случаи это или создание или модификация что тут - одно и то же
mod_conv->SetTransactionFlag(true);
mod_conv->Clear();
int is_work=false;
for(int i=0; i<tree_data.size();i++)
{
    if(tree_data[i].read_choice)
    {
        for(int j=0; j<tree_data[i].file.size(); j++)
        {
            QString file_name=tree_data[i].file[j].file_name;
            if(tree_data[i].file[j].IsFounded==1)
            {
                mod_conv->DeletingLocalFile(file_name);
            }
            else
            {
                mod_conv->ReceiveFile(file_name);
            }
            is_work=true;
        }
    }
}
if(is_work)
{
mod_conv->StartReceiveDeleteFiles();
}
else
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Нет выделеных моделей для чтения"));
    MB.setWindowTitle(QString::fromUtf8("Ошибка"));
    MB.exec();
}
//    mod_conv->SetTransactionFlag(true);
//    mod_conv->Clear();
//    if(listRec.size()>0)
//    {
//        for(int i=0; i<listRec.size(); i++)
//        {
//            QString S=listRec[i];
//            mod_conv->ReceiveFile(S);
//        }
//    }

//    if(listDelLoc.size()>0)
//    {
//        for(int i=0;i<listDelLoc.size(); i++)
//        {
//            QString S=listDelLoc[i];
//            mod_conv->DeletingLocalFile(S);
//        }
//    }

//    mod_conv->StartReceiveDeleteFiles();

}
//----------------------------------------------------------
void MainForm::BuildingTree(const QString& user_login, tTreeMod _direction)
{
    //В таблицу CompareTablesToTree из локальной, ласт и серверной таблиц занести ориентируясь на Struct суммарные хеши моделей
    db_op->WriteToCompareTablesToTree(user_login);

    //Провести анализ на направление изменений моделей в целом
    list_compare=db_op->AnalyzeCompareAll();

    //добавляем к стректуре моделей различающиеся файлы
    db_op->AddFilesToModelsStruct(list_compare);

    //строим дерево и модели помечаем в соответствии с анализом (нулевой результат пропускаем)
    ConstructTree(_direction, list_compare);

}
//----------------------------------------------------------
void MainForm::on_pbBuildRead_clicked()
{
    BuildingTree(user_login, Read);
}
//----------------------------------------------------------
void MainForm::ConstructTree(tTreeMod _tree_mod, QList<CompareTableRec> _comp_table)
{
    tree_data=_comp_table;

        switch (_tree_mod)
        {
        case Read:
        {
            read_tree_model->clear();

            ConstructTreeModel(read_tree_model, true);

            ui->tvRead->setModel(read_tree_model);
            ui->tvRead->setHeaderHidden(true);
            ui->tvRead->collapseAll();

            TreeCustomCollapsed(read_tree_model->invisibleRootItem(), _tree_mod);

            ui->tvRead->setRootIsDecorated(true);
            ui->tvRead->setAnimated(true);
            break;
        }
        case Write:
        {
            write_tree_model->clear();

            ConstructTreeModel(write_tree_model, false);

            ui->tvWrite->setModel(write_tree_model);
            ui->tvWrite->setHeaderHidden(true);
            ui->tvWrite->collapseAll();

            TreeCustomCollapsed(write_tree_model->invisibleRootItem(), _tree_mod);

            ui->tvWrite->setRootIsDecorated(true);
            ui->tvWrite->setAnimated(true);
            break;
        }
        }


}
//----------------------------------------------------------
void MainForm::ConstructTreeModel(QStandardItemModel *_tree_model, bool _read)
{
    for(int i=0; i<tree_data.size(); i++)
    {
        int res=tree_data[i].result;
        qlonglong loc_num_mod=tree_data[i].model_local;
        qlonglong serv_num_mod=tree_data[i].model_server;
        QString mod_struct=tree_data[i].mod_struct;
        QStringList list_model=mod_struct.split("/");

        //        bool read=false;
        QStandardItem *item=_tree_model->invisibleRootItem();


        for(int j=0; j<list_model.size(); j++)
        {
            QString txt=item->text();
            //            log.Write(QString(QString("Ветви уже есть. Значение ")+txt.toUtf8()));
            //ветви уже есть
            //проверить всех потомков текущей ветви
            //если есть совпадение то перейти к этому потомку
            //если нет - создать нового потомка
            bool find=false;
            int row_count=item->rowCount();
            for(int k=0; k<row_count; k++)
            {
                QString ch_text=item->child(k)->text();
                QString mod_text=list_model[j];
                //                log.Write(QString(QString("Сверяю потомков. Ветвь: ")+ch_text.toUtf8()+QString(" Модель: ")+mod_text.toUtf8()));

                if(ch_text==mod_text)
                {
                    //                    log.Write(QString("Потомок найден "));
                    Qt::CheckState st=item->child(k)->checkState();
//                    item->setCheckable(true);

//                    item->child(k)->setCheckState(Qt::Unchecked);
                    //                            log.Write(QString("Установлено - выключено "));

                    item=item->child(k);
                    QString t=item->text();
                    find=true;
                    break;
                }
            }
            if(!find)
            {
//                QString T=list_model[j];
                QStandardItem *new_item=new QStandardItem(list_model[j]);
                QFont font;
                if(j==list_model.size()-1)
                {

                    font.setBold(true);

                    if(res>0)
                    {
                        new_item->setData(res,Qt::UserRole+1);
                        new_item->setData(loc_num_mod,Qt::UserRole+2);
                        new_item->setData(serv_num_mod,Qt::UserRole+3);
                        if(res>0)
                        {
//                        new_item->setCheckable(true);
//                        new_item->setCheckState(Qt::Unchecked);

                        switch(res)
                        {
                        case 1:
                        {
//                            new_item->setCheckable(true);
                            new_item->setCheckState(Qt::Unchecked);
                            new_item->setCheckable(true);
                            new_item->setTristate(true);
                            new_item->setSelectable(false);
                            new_item->setEditable(false);
                            QIcon icon(":/Icons/Tree/Triangle_right.ico");
                            new_item->setIcon(icon);
                            break;
                        }
                        case 2:
                        {
//                            new_item->setCheckable(true);
                            new_item->setCheckState(Qt::Unchecked);
                            new_item->setCheckable(true);
                            new_item->setTristate(true);
                            new_item->setSelectable(false);
                            new_item->setEditable(false);
                            QIcon icon(":/Icons/Tree/Triangle_left.ico");
                            new_item->setIcon(icon);
                            break;
                        }
                        case 3:
                        {
//                            new_item->setCheckable(true);
                            new_item->setCheckState(Qt::Unchecked);
                            new_item->setCheckable(true);
                            new_item->setTristate(true);
                            new_item->setSelectable(false);
                            new_item->setEditable(false);
                            if(_read)
                            {
                            QIcon icon(":/Icons/Tree/New_sm.ico");
                            new_item->setIcon(icon);
                            }
                            else
                            {
                            QIcon icon(":/Icons/Tree/Del_sm.ico");
                            new_item->setIcon(icon);
                            }

                            break;
                        }
                        case 4:
                        {
//                            new_item->setCheckable(true);
                            new_item->setCheckState(Qt::Unchecked);
                            new_item->setCheckable(true);
                            new_item->setTristate(true);
                            new_item->setSelectable(false);
                            new_item->setEditable(false);
                            if(_read)
                            {
                            QIcon icon(":/Icons/Tree/Del_sm.ico");
                            new_item->setIcon(icon);
                            }
                            else
                            {
                            QIcon icon(":/Icons/Tree/New_sm.ico");
                            new_item->setIcon(icon);
                            }

                            break;
                        }
                        }
                        }

                    QList<tFile> tf=tree_data[i].file;
                    QStandardItem *file_group=new QStandardItem(QString::fromUtf8("Файлы"));
                    file_group->setSelectable(false);
                    file_group->setEditable(false);
                    for(int l=0; l<tf.size();l++)
                    {
                        int is_founded=tree_data[i].file[l].IsFounded;
                        QString name=tree_data[i].file[l].file_name;
                        QString last_mod1=tree_data[i].file[l].last_mod;
                        int char_t=last_mod1.indexOf("T");
                        QString last_mod=last_mod1.left(char_t)+" "+last_mod1.right(last_mod1.length()-char_t-1);
                        bool local=tree_data[i].file[l].Local;
                        qlonglong num=tree_data[i].file[l].num;
                        qlonglong size=tree_data[i].file[l].size;

                        QStandardItem *file=new QStandardItem(name);
                        file->setSelectable(false);
                        file->setEditable(false);
                        QFont file_font;
//                        QBrush file_brush;
//                        QColor color(128,128,0,0);
//                        file_brush.setColor(color);
                        file_font.setItalic(true);
                        file_font.setBold(true);

                        int size_font=file_font.pointSize();

                        file->setFont(file_font);

//                        file->setBackground(file_brush);

                        QFont file_prop_font;
                        file_prop_font.setPointSize(size_font*0.95);
                        QList<QStandardItem *> columns;
                        QStandardItem *col1=new QStandardItem("Размер: "+QString::number(size)+" bytes");
                        file_prop_font.setItalic(true);
                        col1->setFont(file_prop_font);
                        col1->setSelectable(false);
                        col1->setEditable(false);

                        QStandardItem *col2=new QStandardItem(QString::fromUtf8("Изменено: ")+last_mod);
                        col2->setFont(file_prop_font);
                        col2->setSelectable(false);
                        col2->setEditable(false);

                        col1->setBackground(QBrush(QColor(255,255,0, 64)));
                        col2->setBackground(QBrush(QColor(255,255,0, 64)));
                        columns.push_back(col1);
                        columns.push_back(col2);

                        file->appendColumn(columns);
                        //                QIcon icon(":/Icons/Tree/Del_sm.ico");
                        //                new_item->setIcon(icon);
                        if(_read)
                        {
                            //режим чтения
                            //флаг IsFounded (1 - есть только на локальном, 2 - есть только на серверном, 0 - есть и там и там)
                            //при чтении если файл только на локальном то при чтении он будет удален и значит иконка удаления
                            //если только на сервере то иконка добавления
                            //при записи наоборот

                            switch(is_founded)
                            {
                            case 1:
                            {

                                QIcon icon(":/Icons/Tree/Del_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                QIcon icon(":/Icons/Tree/New_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            }

                        }
                        else
                        {
                            //режим записи
                            switch(is_founded)
                            {
                            case 1:
                            {


                                QIcon icon(":/Icons/Tree/New_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                 QIcon icon(":/Icons/Tree/Del_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            }
                        }

                        file_group->appendRow(file);

                    }
                    new_item->appendRow(file_group);
                    }
                    else
                    {
                        //col1->setBackground(QBrush(QColor(255,255,0, 64)));
//                        new_item->setCheckable(false);
                        new_item->setData(-2,Qt::UserRole+1);
                        new_item->setData(loc_num_mod,Qt::UserRole+2);
                        new_item->setData(serv_num_mod,Qt::UserRole+3);
                        new_item->setBackground(QBrush(QColor(255,0,0, 192)));
                    }
                }
                else
                {
//                    new_item->setCheckable(true);
                    new_item->setCheckState(Qt::Unchecked);
                    new_item->setCheckable(true);
                    new_item->setTristate(true);
                    new_item->setSelectable(false);
                    new_item->setEditable(false);
                    new_item->setData(-1, Qt::UserRole+1);
                    font.setBold(false);
                }
                new_item->setFont(font);

                new_item->setTristate(true);


                //                log.Write(QString("Новая ветвь. Установлено - выключено "+new_item->text().toUtf8()));

                new_item->setEditable(false);
                new_item->setSelectable(false);
                //                QIcon icon(":/Icons/Tree/Del_sm.ico");
                //                new_item->setIcon(icon);

                item->appendRow(new_item);

                item=new_item;
            }
            else
            {
                QFont font;
                if(j==list_model.size()-1)
                {
//                    item->setCheckable(true);
                    font.setBold(true);

                    if(res>0)
                    {
                        item->setData(res,Qt::UserRole+1);
                        item->setData(loc_num_mod,Qt::UserRole+2);
                        item->setData(serv_num_mod,Qt::UserRole+3);
                        if(res>0)
                        {
//                        item->setCheckable(true);
//                        item->setCheckState(Qt::Unchecked);

                        switch(res)
                        {
                        case 1:
                        {
//                            item->setCheckable(true);
                            item->setCheckState(Qt::Unchecked);
                            item->setCheckable(true);
                            item->setTristate(true);
                            item->setSelectable(false);
                            item->setEditable(false);
                            QIcon icon(":/Icons/Tree/Triangle_right.ico");
                            item->setIcon(icon);
                            break;
                        }
                        case 2:
                        {
//                            item->setCheckable(true);
                            item->setCheckState(Qt::Unchecked);
                            item->setCheckable(true);
                            item->setTristate(true);
                            item->setSelectable(false);
                            item->setEditable(false);
                            QIcon icon(":/Icons/Tree/Triangle_left.ico");
                            item->setIcon(icon);
                            break;
                        }
                        case 3:
                        {
//                            item->setCheckable(true);
                            item->setCheckState(Qt::Unchecked);
                            item->setCheckable(true);
                            item->setTristate(true);
                            item->setSelectable(false);
                            item->setEditable(false);
                            if(_read)
                            {
                            QIcon icon(":/Icons/Tree/New_sm.ico");
                            item->setIcon(icon);
                            }
                            else
                            {
                            QIcon icon(":/Icons/Tree/Del_sm.ico");
                            item->setIcon(icon);
                            }

                            break;
                        }
                        case 4:
                        {
//                            item->setCheckable(true);
                            item->setCheckState(Qt::Unchecked);
                            item->setCheckable(true);
                            item->setTristate(true);
                            item->setSelectable(false);
                            item->setEditable(false);
                            if(_read)
                            {
                            QIcon icon(":/Icons/Tree/Del_sm.ico");
                            item->setIcon(icon);
                            }
                            else
                            {
                            QIcon icon(":/Icons/Tree/New_sm.ico");
                            item->setIcon(icon);
                            }

                            break;
                        }
                        }
                        }

                    QList<tFile> tf=tree_data[i].file;
                    QStandardItem *file_group=new QStandardItem(QString::fromUtf8("Файлы"));
                    file_group->setSelectable(false);
                    file_group->setEditable(false);
                    for(int l=0; l<tf.size();l++)
                    {
                        int is_founded=tree_data[i].file[l].IsFounded;
                        QString name=tree_data[i].file[l].file_name;
                        QString last_mod1=tree_data[i].file[l].last_mod;
                        int char_t=last_mod1.indexOf("T");
                        QString last_mod=last_mod1.left(char_t)+" "+last_mod1.right(last_mod1.length()-char_t-1);
                        bool local=tree_data[i].file[l].Local;
                        qlonglong num=tree_data[i].file[l].num;
                        qlonglong size=tree_data[i].file[l].size;

                        QStandardItem *file=new QStandardItem(name);
                        QFont file_font;
//                        QBrush file_brush;
//                        QColor color(128,128,0,0);
//                        file_brush.setColor(color);
                        file_font.setItalic(true);
                        file_font.setBold(true);

                        int size_font=file_font.pointSize();

                        file->setFont(file_font);

//                        file->setBackground(file_brush);

                        QFont file_prop_font;
                        file_prop_font.setPointSize(size_font*0.95);
                        QList<QStandardItem *> columns;
                        QStandardItem *col1=new QStandardItem("Размер: "+QString::number(size)+" bytes");
                        file_prop_font.setItalic(true);
                        col1->setFont(file_prop_font);
                        col1->setSelectable(false);
                        col1->setEditable(false);

                        QStandardItem *col2=new QStandardItem(QString::fromUtf8("Изменено: ")+last_mod);
                        col2->setFont(file_prop_font);
                        col2->setSelectable(false);
                        col2->setEditable(false);

                        col1->setBackground(QBrush(QColor(255,255,0, 64)));
                        col2->setBackground(QBrush(QColor(255,255,0, 64)));
                        columns.push_back(col1);
                        columns.push_back(col2);

                        file->appendColumn(columns);
                        //                QIcon icon(":/Icons/Tree/Del_sm.ico");
                        //                new_item->setIcon(icon);
                        if(_read)
                        {
                            //режим чтения
                            //флаг IsFounded (1 - есть только на локальном, 2 - есть только на серверном, 0 - есть и там и там)
                            //при чтении если файл только на локальном то при чтении он будет удален и значит иконка удаления
                            //если только на сервере то иконка добавления
                            //при записи наоборот

                            switch(is_founded)
                            {
                            case 1:
                            {

                                QIcon icon(":/Icons/Tree/Del_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                QIcon icon(":/Icons/Tree/New_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            }

                        }
                        else
                        {
                            //режим записи
                            switch(is_founded)
                            {
                            case 1:
                            {


                                QIcon icon(":/Icons/Tree/New_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                 QIcon icon(":/Icons/Tree/Del_sm.ico");
                                file->setIcon(icon);
                                break;
                            }
                            }
                        }

                        file_group->appendRow(file);

                    }
                    item->appendRow(file_group);
                    }
                    else
                    {
                        //col1->setBackground(QBrush(QColor(255,255,0, 64)));
//                        item->setCheckable(false);
                        item->setData(-2,Qt::UserRole+1);
                        item->setData(loc_num_mod,Qt::UserRole+2);
                        item->setData(serv_num_mod,Qt::UserRole+3);
                        item->setBackground(QBrush(QColor(255,0,0, 192)));
                    }
                }
            }
        }
    }

}
//----------------------------------------------------------
void MainForm::on_tvRead_clicked(const QModelIndex &index)
{
    UpToParentFiles(read_tree_model, index, read_tree_model->itemFromIndex(index)->checkState());
    DownToChildrensFiles(read_tree_model, index, read_tree_model->itemFromIndex(index)->checkState(), Read);
//    int row=index.row();


//    int res=-1000;
//    res=read_tree_model->itemData(index).value(Qt::UserRole+1).toInt();

//    qlonglong num_loc_mod=0;
//    qlonglong num_serv_mod=0;

//    if(res!=0)
//    {
//        num_loc_mod=read_tree_model->itemData(index).value(Qt::UserRole+2).toInt();
//        num_serv_mod=read_tree_model->itemData(index).value(Qt::UserRole+3).toInt();
//    }
}
//----------------------------------------------------------
void MainForm::TreeCustomCollapsed(QStandardItem *item, tTreeMod _tree_mod)
{
    //все ветви с UserRole+1 меньше нуля - развернуть
    //    QStandardItem *item=_tree_model->invisibleRootItem();
    int row_count=item->rowCount();
    QString  txt=item->text();
    //    QList<QStandardItem *> item_list;

    for(int k=0; k<row_count; k++)
    {
        int prop=item->child(k)->data(Qt::UserRole+1).toLongLong();
        QString ch_txt=item->child(k)->text();
        int par_prop=item->data(Qt::UserRole+1).toLongLong();
        QString par_txt=item->text();
        if(prop<0 || txt=="")
        {
            QModelIndex index=item->child(k)->index();
            switch (_tree_mod)
            {
            case Read:
            {
                ui->tvRead->expand(index);
                break;
            }
            case Write:
            {
                ui->tvWrite->expand(index);
                break;
            }
            }

            ui->tvRead->expand(index);
        }
//        if(prop>0 && par_prop<0 && txt!="Файлы")
//        {

//            ui->tvRead->expand(item->child(k)->index());
//        }
        int ch_row_count=item->child(k)->rowCount();
        bool is_submod=false;
        for(int j=0; j<ch_row_count; j++)
        {
            int prop_ch=item->child(k)->child(j)->data(Qt::UserRole+1).toLongLong();
            if(prop_ch>0)
            {
                is_submod=true;
                break;
            }
        }
        if(is_submod)
        {
            switch(_tree_mod)
            {
            case Read:
            {
                ui->tvRead->expand(item->child(k)->index());
                break;
            }
            case Write:
            {
                ui->tvWrite->expand(item->child(k)->index());
                break;
            }
            }


        }

        TreeCustomCollapsed(item->child(k), _tree_mod);
    }
}
//----------------------------------------------------------
void MainForm::EndUpdateServerModel()
{
    //формирование дерева чтения по полученым и имеющимся данным после обновления данных с сервера

    OnListFilesLocal();

//    BuildingReadTree(user_login, Read);
//    BuildingReadTree(user_login, Write);
}
//----------------------------------------------------------
void MainForm::UpToParentFiles(QStandardItemModel *model, const QModelIndex &index, Qt::CheckState _state)
{
    const QModelIndex parent_index=model->parent(index);
    QStandardItem *parent_item=model->itemFromIndex(parent_index);
    Qt::CheckState state=_state;
    if(!parent_item)
    {
        return;
    }
    else
    {
        if(parent_item->data(Qt::UserRole+1).toLongLong()==-1)
        {
        QString parent_txt=parent_item->text();
        int child_count=parent_item->rowCount();

        if(child_count>1)
        {
            for(int i=0; i<child_count;i++)
            {
                QStandardItem *child_item=parent_item->child(i);
                if (child_item->isCheckable())
                {
                    QString child_txt=child_item->text();

                    switch (child_item->checkState())
                    {
                    case Qt::Checked:
                    {
                        if(state!=Qt::Checked)
                        {
                            state=Qt::PartiallyChecked;
                        }
                        break;
                    }
                    case Qt::Unchecked:
                    {
                        if(state!=Qt::Unchecked)
                        {
                            state=Qt::PartiallyChecked;
                        }
                        break;
                    }
                    case Qt::PartiallyChecked:
                    {
                        state=Qt::PartiallyChecked;
                        break;
                    }
                    }
                }
                if(state==Qt::PartiallyChecked)
                {
                    break;
                }
            }
        }
        else
        {
            qlonglong s_num=parent_item->data(Qt::UserRole+1).toLongLong();
            if(s_num>0)
            {
                state=Qt::Checked;

            }
        }
        parent_item->setCheckState(state);
        }
        UpToParentFiles(model, parent_index, state);

    }
}
//----------------------------------------------------------
void MainForm::DownToChildrensFiles(QStandardItemModel *model, QModelIndex index, Qt::CheckState _state, tTreeMod _direction)
{
    QStandardItem *item=model->itemFromIndex(index);
    QString txt=item->text();
    qlonglong s_num=item->data(Qt::UserRole+1).toLongLong();
    if(s_num>0 && s_num!=0)
    {
        qlonglong loc_num_mod=item->data(Qt::UserRole+2).toLongLong();
        qlonglong serv_num_mod=item->data(Qt::UserRole+3).toLongLong();

        //обновить таблицу ModelRead
        bool state=false;
        switch (_direction)
        {
        case Read:
        {
            if(_state==Qt::Checked)
            {
                state=true;
                //тут ставим true в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, true, true);
            }
            else
            {
                //тут ставим false в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, true, false);
            }
            break;
        }
        case Write:
        {
            if(_state==Qt::Checked)
            {
                state=true;
                //тут ставим true в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, false, true);
            }
            else
            {
                //тут ставим false в структуре для модели
                SetToModelsTreeData(loc_num_mod, serv_num_mod, false, false);
            }
            break;
        }
        }


//        db_op->SaveReadPermission(sLM_Logins->stringList().value(admin_logins_index.row()), s_num, state);
    }

    int child_count=item->rowCount();
    if(child_count==0)
    {
        return;
    }
    else
    {
        for(int i=0; i<child_count; i++)
        {
            QStandardItem *children=item->child(i);
            QString ch_txt=children->text();
            if(children->text()!="Файлы")
            {
                qlonglong d=children->data(Qt::UserRole+1).toLongLong();
                if(d!=-2)
                {
                    children->setCheckState(_state);

                }
            QModelIndex child_index=children->index();
            DownToChildrensFiles(model, child_index, _state, _direction);
            }
        }
    }
}
//----------------------------------------------------------
void MainForm::SetToModelsTreeData(qlonglong loc_num_model, qlonglong _serv_num_model, bool _to_read, bool _choice)
{
//    struct tFile
//    {
//        bool Local; // 1 - локальный файл, 0 - серверный файл
//        qlonglong num;
//        QString file_name;
//        qint64 size;
//        QString last_mod;
//        int IsFounded;

//    } ;

//    struct CompareTableRec
//    {
//        qlonglong num;
//        qlonglong model_local;
//        qlonglong model_server;
//        QString mod_struct;
//        int result;
//        QList<tFile> file;
//        bool read_choice;
//        bool write_choice;
//    };
    for(int i=0; i<tree_data.size(); i++)
    {
        if(tree_data[i].model_local==loc_num_model && tree_data[i].model_server==_serv_num_model)
        {
            //модель найдена
            if(_to_read)
            {
                tree_data[i].read_choice=_choice;
            }
            else
            {
                tree_data[i].write_choice=_choice;
            }
        }
    }
}
//----------------------------------------------------------
void MainForm::on_pbRefreshRead_clicked()
{
    //Обновление серверных таблиц

    IsRequeryServerModel=true;
    OnListFiles();
}
//----------------------------------------------------------
void MainForm::on_pbRefresh_Write_clicked()
{
    IsRequeryServerModel=true;
    OnListFiles();
}
//----------------------------------------------------------
void MainForm::on_pbBuildWrite_clicked()
{
    BuildingTree(user_login, Write);
}
//----------------------------------------------------------
void MainForm::on_tvWrite_clicked(const QModelIndex &index)
{
    UpToParentFiles(write_tree_model, index, write_tree_model->itemFromIndex(index)->checkState());
    DownToChildrensFiles(write_tree_model, index, write_tree_model->itemFromIndex(index)->checkState(), Write);
}
//----------------------------------------------------------
void MainForm::on_pbWrite_clicked()
{
mod_conv->SetTransactionFlag(true);
mod_conv->Clear();
int is_work=false;
for(int i=0; i<tree_data.size();i++)
{
    if(tree_data[i].write_choice)
    {
        for(int j=0; j<tree_data[i].file.size(); j++)
        {
            QString file_name=tree_data[i].file[j].file_name;
            if(tree_data[i].file[j].IsFounded==2)
            {
                mod_conv->DeletingServerFile(file_name);
            }
            else
            {
                mod_conv->SendFile(file_name);
            }
            is_work=true;
        }
    }
}
if(is_work)
{
mod_conv->StartSendDeleteFiles();
}
else
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Нет выделеных моделей для записи"));
    MB.setWindowTitle(QString::fromUtf8("Ошибка"));
    MB.exec();
}
}
//----------------------------------------------------------
void MainForm::showContextMenuRead(QPoint pos)
{
    QPoint global_pos;
    if(sender()->inherits("QAbstractScrollArea"))
    {
        global_pos=((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(pos);
    }
    else
    {
        global_pos=ui->tvRead->mapToGlobal(pos);
    }

    QModelIndex click_index=ui->tvRead->indexAt(pos);
    bool valid=click_index.isValid();

    if(valid)
    {

        QStandardItem *item=read_tree_model->itemFromIndex(click_index);
        QString txt=item->text();
        qlonglong d=item->data(Qt::UserRole+1).toLongLong();
        if(d==-2)
        {
            qlonglong local_num=item->data(Qt::UserRole+2).toLongLong();
            qlonglong server_num=item->data(Qt::UserRole+3).toLongLong();
            QMenu menu;

            QAction *action1=new QAction(QString::fromUtf8("Принять актуальной локальную версию"), this);
            action1->setData(1);
            menu.addAction(action1);

            QAction *action2=new QAction(QString::fromUtf8("Принять актуальной серверную версию"), this);
            action2->setData(2);
            menu.addAction(action2);

            QAction* selectedItem=menu.exec(global_pos);

            if(selectedItem)
            {
                int menu_id=selectedItem->data().toInt();
                switch(menu_id)
                {
                case 1:
                {
                    db_op->ActualiseModel(user_login, server_num, true);
                    BuildingTree(user_login, Read);
                    break;
                }
                case 2:
                {
                    db_op->ActualiseModel(user_login, local_num, true);
                    BuildingTree(user_login, Read);
                    break;
                }
                }
            }
        }


    }
}
