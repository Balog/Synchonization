#include "MainForm.h"
#include "ui_MainForm.h"
#include <QFileDialog>
#include "tLog.h"
#include <QRegExp>


extern tSettings my_settings;

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent),mod_conv(NULL), db_op(NULL),
    sLM_loc_list_models(NULL), slm_server_list_models(NULL), slm_list(NULL),
    login_pass(new tEditLoginPass), adm_tree_model(NULL)
{
//    constr_mod_tree=NULL;
    IsRequeryServerModel=false;

    sLM_Send=new QStringListModel;
    sLM_Del=new QStringListModel;
    sLM_Rec=new QStringListModel;
    sLM_DelLoc=new QStringListModel;
    sLM_Logins=new QStringListModel;

    adm_tree_model= new QStandardItemModel();;

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

//    UpdateLogins();


    tLog log;

    log.Write(tr("Начало работы клиента"));
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{
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
void MainForm::UpdateLoginsTable(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateLogins(_block);

    UpdateLogins();


    OnListFilesLocal();

    OnListFiles();

//    emit StartAutorizForm();

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

//        if(read)
//        {
//            log.Write(QString("Следующая модель read=true"));
//        }
//        else
//        {
//            log.Write(QString("Следующая модель read=false"));
//        }

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
                            log.Write(QString("Установлено - включено "));
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
//    QStandardItem *item=adm_tree_model->itemFromIndex(index);
//    Qt::CheckState state=item->checkState();

//    const QModelIndex parent_index=adm_tree_model->parent(index);
//    QStandardItem *parent_item=adm_tree_model->itemFromIndex(parent_index);

//    QObjectList childrens=adm_tree_model->children();
//    int count_ch=childrens.size();

//    QString item_text=item->text();
//    QString parent_text=parent_item->text();

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
    //(как это сделать пока не знаю)

    UpToParent(index, adm_tree_model->itemFromIndex(index)->checkState());
    DownToChildrens(index, adm_tree_model->itemFromIndex(index)->checkState());

}
//----------------------------------------------------------
void MainForm::UpdateModelRead(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateModelRead(_block);

    StartAutoriz();

}
//----------------------------------------------------------

void MainForm::on_lvLogins_clicked(const QModelIndex &index)
{
    //    QModelIndex MI=ui->lvLocalListFiles->currentIndex();

    //    int N=MI.row();
    //    if(N<0)
    //    {
    //        QMessageBox MB;
    //        MB.setText(QString::fromUtf8("Выделите клиентскую модель"));
    //        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
    //        MB.exec();
    //    }
    //    else
    //    {
    //        QStringListModel *M=new QStringListModel;
    //        M=(QStringListModel *)MI.model();

    //        QString S=M->stringList().value(N);

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
    IsRequeryServerModel=true;
    OnListFiles();
}
