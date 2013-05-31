#include "MainForm.h"
#include "ui_MainForm.h"
#include <QFileDialog>
#include "tLog.h"
#include <QRegExp>
#include <QMenu>

extern tSettings my_settings;

Qt::ItemFlags TableModel::flags ( const QModelIndex & index ) const
{
    Qt::ItemFlags result = QStandardItemModel::flags(index);
    if (index.column()==0) result |= Qt::ItemIsUserCheckable;
    return result;
}

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint),
    mod_conv(NULL), sLM_Logins(new QStringListModel), adm_tree_model(new QStandardItemModel), tableModel(new TableModel),
    login_pass(new tEditLoginPass), form_new_path(new tNewPath),
    read_tree_model(new QStandardItemModel()), write_tree_model(new QStandardItemModel()),
    fProgress(new tProgress),table_files_model(NULL), previews(NULL),db_op(new tDatabaseOp),  current_local_model(0),
    IsRequeryServerModel(false), _user_login("")
{


    ui->setupUi(this);

    ui->tabWidget->removeTab(3);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget_2->setCurrentIndex(0);
    ui->tabWidget_3->setCurrentIndex(0);

    ui->leAddr->setText(my_settings.GetServerAddr());
    ui->sbPort->setValue(my_settings.GetServerPort());

    my_settings.SetServerAddr(ui->leAddr->text());
    my_settings.SetServerPort(ui->sbPort->value());
    my_settings.sync();

    mod_conv= new tModelsConveyor(this, db_op);

    mod_conv->StartServer(my_settings.GetServerAddr(), my_settings.GetServerPort());

    fProgress->setVisible(false);
    login_pass->setVisible(false);
    connect(login_pass, SIGNAL(EndEditing(QString&,QString&,int,bool)), this, SLOT(OnEndEditLoginPassword(QString&,QString&,int,bool)));
    connect(form_new_path, SIGNAL(ContinueStrat()), this, SLOT(OnContinueStart()));

    connect(mod_conv, SIGNAL(SignalCountFiles(int)), fProgress, SLOT(setValue(int)));
    connect(this, SIGNAL(ProgressStart(int, int, int, int, int)), fProgress, SLOT(Start(int, int, int, int, int)));
    connect(this, SIGNAL(ProgressStop()), fProgress, SLOT(Stop()));

    ui->lvLogins->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tvRead->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tvWrite->setContextMenuPolicy(Qt::CustomContextMenu);

    tLog log;
    log.Write(tr("Начало работы клиента"));
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{
    SaveDescriptionModel(ui->pteDesRead->toPlainText());

    delete form_new_path;
    delete login_pass;

    delete sLM_Logins;
    delete tableModel;

    delete adm_tree_model;
    delete read_tree_model;
    delete write_tree_model;
    delete fProgress;

    delete mod_conv;
    mod_conv=NULL;

    delete db_op;
    db_op=NULL;

    delete table_files_model;

    delete ui;

    tLog log;
    log.Write(tr("Конец работы клиента"));
}
//---------------------------------------------------------------------
void MainForm::Autorization(QString& _login, QString& _password,  bool _modify_folder)
{
    _user_login=_login;
    this->setWindowTitle(QString::fromUtf8(QString("Главная форма (Пользователь '"+_user_login+"')").toAscii()));
    modify_folder=_modify_folder;
    mod_conv->SetLogin(_user_login);
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

    BuildingTree(_user_login);
    //строим дерево и модели помечаем в соответствии с анализом (нулевой результат пропускаем)
    ConstructTree(Read, list_compare);
    ConstructTree(Write, list_compare);

    DisplayInfo(_current_local_num, _current_server_num);
    emit ProgressStop();
    tLog log;
    log.Write(tr("Работа завершена"));


}
//---------------------------------------------------------------------
void MainForm::OnAutorizStart()
{
    emit AutorizStart();
}
//---------------------------------------------------------------------
void MainForm::OnSetVisible(const bool _vis)
{
    this->setVisible(_vis);
    ui->pbConnect->setEnabled(false);
    ui->pbDisconnect->setEnabled(true);
}
//---------------------------------------------------------------------
void MainForm::OnDisconnecting()
{
    emit Disconnecting();
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

    mod_conv->StartSendDeleteFiles(max_models);

}
//---------------------------------------------------------------------
void MainForm::OnClearSendAll()
{
    listSend.clear();
}
//---------------------------------------------------------------------
void MainForm::OnClearDelete()
{
    listDel.clear();
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
void MainForm::OnReceiveClear()
{
    listRec.clear();
}
//---------------------------------------------------------------------
void MainForm::OnClearDeleteLocal()
{
    listDelLoc.clear();

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

    mod_conv->StartReceiveDeleteFiles(my_settings.GetRoot(), 0, max_models);


}
//---------------------------------------------------------------------
void MainForm::OnListFilesLocal()
{
    db_op->RefreshModelsFiles();

    VerifyLastTable(_user_login);
}
//---------------------------------------------------------------------
void MainForm::CreateFileList(const QString &_start_folder, QStringList& _list)
{
    QDir dir(_start_folder);
    SearchFiles(dir, _list);
}
//----------------------------------------------------------
void MainForm::SearchFiles(const QDir & _dir, QStringList &list)
{
    QStringList lstDirs = _dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lstFiles = _dir.entryList(QDir::Files | QDir::Hidden);

    //записываем файлы
    foreach (QString entry, lstFiles)
    {
        QString entryAbsPath = _dir.absolutePath() + "/" + entry;
        list.push_back(entryAbsPath);
    }

    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lstDirs)
    {
        QString entryAbsPath = _dir.absolutePath() + "/" + entry;
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

void MainForm::SaveServerModelFiles(QByteArray &_block)
{
//    //распарсить переданый блок и записать в папки серверного состояния моделей и их файлов
    db_op->SaveServerModelFiles(_block);
}
//----------------------------------------------------------
void MainForm::CorrectLastSynch(const bool _server)
{

    mod_conv->MarkLastTables(_server, _user_login);

    db_op->ExecUpdateLastSynch(_server, _user_login);
    mod_conv->ClearAllList();
}
//----------------------------------------------------------
void MainForm::OnNewLogin()
{
    login_pass->setModal(true);
    login_pass->setVisible(true);
    login_pass->new_user=true;
    login_pass->row=-1;
    login_pass->SetLogin(QString(""));


}
//----------------------------------------------------------
void MainForm::OnEditLogin()
{
    QModelIndex MI=ui->lvLogins->currentIndex();
    int N=MI.row();
    if(N>=0)
    {
        QString S=tableModel->data(MI, Qt::EditRole).toString();

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
void MainForm::OnEndEditLoginPassword(QString& _login,  QString& _password,  int _row,  bool _new_user)
{
    if(_login.length()>0 && _password.length()>0)
    {
        QRegExp exp;
        exp.setPattern("^[a-zA-Z0-9]+$");

        if(exp.indexIn(_login)>=0)
        {
            //Логин и пароль проверены на клиенте и отправляются на сервер для дальнейшей проверки в БД и регистрации

            mod_conv->SendLoginPassword(_login, _password, _row, _new_user);
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
void MainForm::OnLoginsClicked(const QModelIndex _ind)
{
    int N=_ind.row();
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
    admin_logins_index=_ind;
    QModelIndex MI=ui->lvLogins->currentIndex();
    QString S=tableModel->data(MI, Qt::EditRole).toString();

    TreesBuildings(S);

    mod_conv->SaveLoginWritable(tableModel,N);
}
//----------------------------------------------------------
void MainForm::RegisterUser(const qlonglong _s_num)
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
    db_op->GetLoginsModel(tableModel);

    ui->lvLogins->setModel(tableModel);
}
//----------------------------------------------------------
void MainForm::DeleteUser(const qlonglong _s_num)
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
void MainForm::TreesBuildings(const QString& _login)
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

        for(int i=0; i<list_model.size(); i++)
        {
            QString txt=item->text();

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

                if(ch_text==mod_text)
                {
                    Qt::CheckState st=item->child(j)->checkState();

                    switch (st)
                    {
                    case Qt::Checked:
                    {
                        if(!read)
                        {
                            item->child(j)->setCheckState(Qt::PartiallyChecked);
                        }
                        else
                        {
                            item->child(j)->setCheckState(Qt::Checked);
                        }
                        break;
                    }
                    case Qt::Unchecked:
                    {
                        if(read)
                        {
                            item->child(j)->setCheckState(Qt::PartiallyChecked);
                        }
                        else
                        {
                            item->child(j)->setCheckState(Qt::Unchecked);
                        }
                        break;
                    }
                    case Qt::PartiallyChecked:
                    {
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
                new_item->setFont(font);
                new_item->setCheckable(true);
                new_item->setTristate(true);
                if(read)
                {
                    new_item->setCheckState(Qt::Checked);
                }
                else
                {
                    new_item->setCheckState(Qt::Unchecked);
                }
                new_item->setEditable(false);
                new_item->setSelectable(true);
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
void MainForm::on_tvAdminTree_clicked(const QModelIndex &_index)
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


    UpToParent(_index, adm_tree_model->itemFromIndex(_index)->checkState());
    DownToChildrens(_index, adm_tree_model->itemFromIndex(_index)->checkState());

    QModelIndex MI=ui->lvLogins->currentIndex();
    QString S=tableModel->data(MI, Qt::EditRole).toString();
    mod_conv->SavePermissionsToServer(db_op->GetNumLogin(S));
}
//----------------------------------------------------------
void MainForm::UpdateModelRead(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateModelRead(_block);

    this->setVisible(true);
}
//----------------------------------------------------------

void MainForm::UpToParent(const QModelIndex _index, const Qt::CheckState _state)
{

    //определим предка
    const QModelIndex parent_index=adm_tree_model->parent(_index);
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
void MainForm::DownToChildrens(const QModelIndex _index, const Qt::CheckState _state)
{
    QStandardItem *item=adm_tree_model->itemFromIndex(_index);

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

        QModelIndex MI=ui->lvLogins->currentIndex();
        QString S=tableModel->data(MI, Qt::EditRole).toString();
        db_op->SaveReadPermission(S, s_num, state);
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

//void MainForm::on_pbListFiles_clicked()
//{
//    //старая кнопка
//    IsRequeryServerModel=true;
//    OnListFiles();
//}
//----------------------------------------------------------
bool MainForm::VerifyUserFolders() const
{
    QString message="";
    QString project_folder="";
    QString temp_folder="";
    if(!modify_folder && db_op->VerifyUserFolders(_user_login, project_folder, temp_folder, message))
    {

        return true;
    }
    else
    {
        form_new_path->SetDatabase(db_op);
        form_new_path->SetLogin(_user_login);
        form_new_path->SetMessage(message);
        form_new_path->setVisible(true);
        return false;
    }

}
//----------------------------------------------------------
void MainForm::OnContinueStart()
{
    db_op->SaveFoldersToSettings(_user_login);
    ui->leRoot->setText(my_settings.GetRoot());
    ui->leTemp->setText(my_settings.GetTemp());
    bool is_admin_user=false;
    bool is_writable_user=false;
    db_op->GetPermissionsUser(_user_login, is_admin_user, is_writable_user);

    if(!is_admin_user)
    {
        ui->tabWidget->setTabEnabled(2, false);
    }
    else
    {
        ui->tabWidget->setTabEnabled(2, true);
    }

    if(!is_writable_user)
    {
        ui->tabWidget->setTabEnabled(1, false);
    }
    else
    {
        ui->tabWidget->setTabEnabled(1, true);
    }
    ui->tabWidget->setCurrentIndex(0);

    db_op->RefreshModelsFiles();

    OnListFilesLocal();
    OnListFiles();

}
//----------------------------------------------------------
void MainForm::VerifyLastTable(const QString& _user_login)
{
    //Проверить если есть такие модели в таблице Last
    //которых уже нет ни в серверной ни в локальной папке
    db_op->VerifyLastTable(_user_login);

}
//----------------------------------------------------------
void MainForm::on_pbExit_clicked()
{
    this->close();
}
//----------------------------------------------------------


void MainForm::on_pbRead_clicked()
{
    SaveDescriptionModel(ui->pteDesRead->toPlainText());

    DisplayModelInfo(0, 0);
    max_models=0;

    //начать чтение с сервера по новому



    //если запрашиваемый файл есть только локально а на сервере его нет то это удаление файла локально
    //остальные случаи это или создание или модификация что тут - одно и то же
    StartReadModeles(my_settings.GetRoot(), 0, false);

}
//----------------------------------------------------------
void MainForm::BuildingTree(const QString& _user_login)
{
    //В таблицу CompareTablesToTree из локальной, ласт и серверной таблиц занести ориентируясь на Struct суммарные хеши моделей
    db_op->WriteToCompareTablesToTree(_user_login);

    //Провести анализ на направление изменений моделей в целом
    list_compare=db_op->AnalyzeCompareAll();

    //добавляем к стректуре моделей различающиеся файлы
    db_op->AddFilesToModelsStruct(list_compare);



}
//----------------------------------------------------------
void MainForm::ConstructTree(const tTreeMod _tree_mod, QList<CompareTableRec> _comp_table)
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
void MainForm::ConstructTreeModel(const QStandardItemModel *_tree_model, const bool _read)
{
    for(int i=0; i<tree_data.size(); i++)
    {
        int res=tree_data[i].result;
        qlonglong loc_num_mod=tree_data[i].model_local;
        qlonglong serv_num_mod=tree_data[i].model_server;
        QString mod_struct=tree_data[i].mod_struct;
        QStringList list_model=mod_struct.split("/");

        QStandardItem *item=_tree_model->invisibleRootItem();


        for(int j=0; j<list_model.size(); j++)
        {
            QString txt=item->text();
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

                if(ch_text==mod_text)
                {
                    item=item->child(k);
                    QString t=item->text();
                    find=true;
                    break;
                }
            }
            if(!find)
            {
                QStandardItem *new_item=new QStandardItem(list_model[j]);
                new_item->setSelectable(false);
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
                            switch(res)
                            {
                            case 1:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
                                new_item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_right.ico");
                                new_item->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
                                new_item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_left.ico");
                                new_item->setIcon(icon);
                                break;
                            }
                            case 3:
                            {
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
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
                                new_item->setCheckState(Qt::Unchecked);
                                new_item->setCheckable(true);
                                new_item->setTristate(true);
                                new_item->setSelectable(true);
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

                            QString ext=name.right(5);
                            if(ext!=".info" || ext!="files")
                            {
                                QString last_mod1=tree_data[i].file[l].last_mod;
                                int char_t=last_mod1.indexOf("T");
                                QString last_mod=last_mod1.left(char_t)+" "+last_mod1.right(last_mod1.length()-char_t-1);
                                qlonglong size=tree_data[i].file[l].size;

                                QStandardItem *file=new QStandardItem(name);
                                file->setSelectable(true);
                                file->setEditable(false);
                                QFont file_font;
                                file_font.setItalic(true);
                                file_font.setBold(true);

                                int size_font=file_font.pointSize();

                                file->setFont(file_font);

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

                                file_group->setData(-3, Qt::UserRole+1);
                                file->setData(-3, Qt::UserRole+1);
                                col1->setData(-3, Qt::UserRole+1);
                                col2->setData(-3, Qt::UserRole+1);

                                columns.push_back(col1);
                                columns.push_back(col2);

                                file->appendColumn(columns);

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
                        }
                        new_item->appendRow(file_group);
                    }
                    else
                    {
                        new_item->setData(-2,Qt::UserRole+1);
                        new_item->setData(loc_num_mod,Qt::UserRole+2);
                        new_item->setData(serv_num_mod,Qt::UserRole+3);
                        new_item->setBackground(QBrush(QColor(255,0,0, 192)));
                    }
                }
                else
                {
                    new_item->setCheckState(Qt::Unchecked);
                    new_item->setCheckable(true);
                    new_item->setTristate(true);
                    new_item->setEditable(false);
                    new_item->setData(-1, Qt::UserRole+1);
                    font.setBold(false);
                }
                new_item->setFont(font);

                new_item->setTristate(true);

                new_item->setEditable(false);

                item->appendRow(new_item);

                item=new_item;
            }
            else
            {
                QFont font;
                if(j==list_model.size()-1)
                {
                    font.setBold(true);

                    if(res>0)
                    {
                        item->setData(res,Qt::UserRole+1);
                        item->setData(loc_num_mod,Qt::UserRole+2);
                        item->setData(serv_num_mod,Qt::UserRole+3);
                        if(res>0)
                        {
                            switch(res)
                            {
                            case 1:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
                                item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_right.ico");
                                item->setIcon(icon);
                                break;
                            }
                            case 2:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
                                item->setEditable(false);
                                QIcon icon(":/Icons/Tree/Triangle_left.ico");
                                item->setIcon(icon);
                                break;
                            }
                            case 3:
                            {
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
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
                                item->setCheckState(Qt::Unchecked);
                                item->setCheckable(true);
                                item->setTristate(true);
                                item->setSelectable(true);
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

                            QString ext=name.right(5);
                            if(ext!=".info" || ext!="files")
                            {
                                QString last_mod1=tree_data[i].file[l].last_mod;
                                int char_t=last_mod1.indexOf("T");
                                QString last_mod=last_mod1.left(char_t)+" "+last_mod1.right(last_mod1.length()-char_t-1);
                                qlonglong size=tree_data[i].file[l].size;

                                QStandardItem *file=new QStandardItem(name);
                                QFont file_font;
                                file_font.setItalic(true);
                                file_font.setBold(true);

                                int size_font=file_font.pointSize();

                                file->setFont(file_font);

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

                                file_group->setData(-3, Qt::UserRole+1);
                                file->setData(-3, Qt::UserRole+1);
                                col1->setData(-3, Qt::UserRole+1);
                                col2->setData(-3, Qt::UserRole+1);

                                columns.push_back(col1);
                                columns.push_back(col2);

                                file->appendColumn(columns);
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
                        }
                        item->appendRow(file_group);
                    }
                    else
                    {
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
void MainForm::on_tvRead_clicked(const QModelIndex &_index)
{
    UpToParentFiles(read_tree_model, _index, read_tree_model->itemFromIndex(_index)->checkState());
    DownToChildrensFiles(read_tree_model, _index, read_tree_model->itemFromIndex(_index)->checkState(), Read);

    SaveDescriptionModel(ui->pteDesRead->toPlainText());

    QStandardItem *item=read_tree_model->itemFromIndex(_index);
    if(item!=NULL)
    {
        qlonglong d=item->data(Qt::UserRole+1).toLongLong();

        if(d>0 || d==-2)
        {
            _current_local_num=item->data(Qt::UserRole+2).toLongLong();
            _current_server_num=item->data(Qt::UserRole+3).toLongLong();
        }
        else
        {
            _current_local_num=-1;
            _current_server_num=-1;
        }

        ui->pbLoadPreviews->setEnabled( ui->rbSourceServ->isChecked() && (d>0 || d==-2));
        ui->pbLoadPreviews_2->setEnabled( ui->rbSourceServ_2->isChecked() && (d>0 || d==-2));

        switch (d)
        {
        case 3:
        {
            //есть только на сервере
            ui->rbSourseLoc->setEnabled(false);
            ui->rbSourseLoc_2->setEnabled(false);

            ui->rbSourceServ->setEnabled(true);
            ui->rbSourceServ_2->setEnabled(true);

            ui->rbSourceServ->setChecked(true);
            ui->rbSourceServ_2->setChecked(true);

            ui->pbLoadPreviews->setEnabled(true);
            ui->pbLoadPreviews_2->setEnabled(true);

            break;
        }

        case 4:
        {
            //есть только на клиенте
            ui->rbSourseLoc->setEnabled(true);
            ui->rbSourseLoc_2->setEnabled(true);

            ui->rbSourceServ->setEnabled(false);
            ui->rbSourceServ_2->setEnabled(false);

            ui->rbSourseLoc->setChecked(true);
            ui->rbSourseLoc_2->setChecked(true);

            ui->pbLoadPreviews->setEnabled(false);
            ui->pbLoadPreviews_2->setEnabled(false);

            break;
        }
        default:
        {
            ui->rbSourseLoc->setEnabled(true);
            ui->rbSourseLoc_2->setEnabled(true);

            ui->rbSourceServ->setEnabled(true);
            ui->rbSourceServ_2->setEnabled(true);

            break;
        }
        }
        DisplayInfo(_current_local_num, _current_server_num);
    }
    else
    {
        DisplayInfo(0,0);
    }
}
//----------------------------------------------------------
void MainForm::TreeCustomCollapsed(const QStandardItem *_item, const tTreeMod _tree_mod)
{
    int row_count=_item->rowCount();
    QString  txt=_item->text();

    for(int k=0; k<row_count; k++)
    {
        int prop=_item->child(k)->data(Qt::UserRole+1).toLongLong();

        if((prop<0 || txt=="") && prop!=-3)
        {
            QModelIndex index=_item->child(k)->index();
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
        }

        int ch_row_count=_item->child(k)->rowCount();
        bool is_submod=false;
        for(int j=0; j<ch_row_count; j++)
        {
            int prop_ch=_item->child(k)->child(j)->data(Qt::UserRole+1).toLongLong();
            if(prop_ch>0)
            {
                is_submod=true;
                break;
            }
        }
        if(is_submod)
        {
            if(prop!=-3)
            {
                switch(_tree_mod)
                {
                case Read:
                {
                    ui->tvRead->expand(_item->child(k)->index());
                    break;
                }
                case Write:
                {
                    ui->tvWrite->expand(_item->child(k)->index());
                    break;
                }
                }
            }

        }

        TreeCustomCollapsed(_item->child(k), _tree_mod);
    }
}
//----------------------------------------------------------
void MainForm::EndUpdateServerModel(const bool _rebuild)
{
    //формирование дерева чтения по полученым и имеющимся данным после обновления данных с сервера

    OnListFilesLocal();

    BuildingTree(_user_login);

    ConstructTree(Read, list_compare);
    ConstructTree(Write, list_compare);
    if(_rebuild)
    {

        RecoveryTreeIndex();
    }
}
//----------------------------------------------------------
void MainForm::UpToParentFiles(const QStandardItemModel *_model, const QModelIndex &_index, const Qt::CheckState _state)
{
    const QModelIndex parent_index=_model->parent(_index);
    QStandardItem *parent_item=_model->itemFromIndex(parent_index);
    qlonglong p_d=-10;
    if(parent_item!=NULL)
    {
        p_d=parent_item->data(Qt::UserRole+1).toLongLong();
    }
    Qt::CheckState state=_state;

    QStandardItem *item=_model->itemFromIndex(_index);
    qlonglong d=item->data(Qt::UserRole+1).toLongLong();
    if(state==Qt::Unchecked &&  p_d==3)
    {
        return;
    }

    if(d==-2 || d==-3)
    {
        return;
    }
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
        UpToParentFiles(_model, parent_index, state);

    }
}
//----------------------------------------------------------
void MainForm::DownToChildrensFiles(const QStandardItemModel *_model, const QModelIndex _index, const Qt::CheckState _state, const tTreeMod _direction)
{
    QStandardItem *item=_model->itemFromIndex(_index);
    QString txt=item->text();
    qlonglong s_num=item->data(Qt::UserRole+1).toLongLong();
    if(s_num==-3 || s_num==-2)
    {
        return;
    }
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
            qlonglong d=children->data(Qt::UserRole+1).toLongLong();
            QString ch_txt=children->text();
            if(d!=-3)
            {

                if(d!=-2)
                {
                    children->setCheckState(_state);

                }
                QModelIndex child_index=children->index();
                DownToChildrensFiles(_model, child_index, _state, _direction);
            }
        }
    }
}
//----------------------------------------------------------
void MainForm::SetToModelsTreeData(const qlonglong _loc_num_model, const qlonglong _serv_num_model, const bool _to_read, const bool _choice)
{
    for(int i=0; i<tree_data.size(); i++)
    {
        if(tree_data[i].model_local==_loc_num_model && tree_data[i].model_server==_serv_num_model)
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
    SaveDescriptionModel(ui->pteDesRead->toPlainText());
    IsRequeryServerModel=true;
    OnListFiles();
}
//----------------------------------------------------------
void MainForm::SearchIndex(const qlonglong _current_local_num, const qlonglong _current_server_num, QModelIndex &_read_index, QModelIndex &_write_index)
{
    QStandardItemModel *read_model=(QStandardItemModel*)ui->tvRead->model();

    _read_index=SearchIndexToModel(_current_local_num, _current_server_num, read_model);

    QStandardItemModel *write_model=(QStandardItemModel*)ui->tvWrite->model();

    _write_index=SearchIndexToModel(_current_local_num, _current_server_num, write_model);
}

//----------------------------------------------------------
QModelIndex MainForm::SearchIndexToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, const QStandardItemModel *_model)
{

    QStandardItem *res_item=NULL;
    QStandardItem *item=_model->invisibleRootItem();
    bool b=false;
    res_item=SearchItemToModel(_current_local_num, _current_server_num, item, b);

    if(!b)
    {
        return QModelIndex();
    }
    else
    {
        return res_item->index();
    }
}

//----------------------------------------------------------
QStandardItem* MainForm::SearchItemToModel(const qlonglong _current_local_num, const qlonglong _current_server_num, QStandardItem *_item, bool &_stop)
{
QStandardItem *res=NULL;


if(_item==NULL)
{
    return res;
}
    qlonglong loc_num=_item->data(Qt::UserRole+2).toLongLong();
    qlonglong serv_num=_item->data(Qt::UserRole+3).toLongLong();
    QString item_text=_item->data(Qt::EditRole).toString();
    if(loc_num==_current_local_num && serv_num==_current_server_num)
    {
        _stop=true;
        res=_item;
        return res;
    }


int count_children=_item->rowCount();
for(int i=0; i<count_children; i++)
{
    if(_stop)
    {
        return res;
    }

    res=SearchItemToModel(_current_local_num, _current_server_num, _item->child(i), _stop);
}
return res;

}

//----------------------------------------------------------
void MainForm::on_pbRefresh_Write_clicked()
{
    SaveDescriptionModel(ui->pteDesRead_2->toPlainText());
    IsRequeryServerModel=true;
    OnListFiles();
}
//----------------------------------------------------------
void MainForm::on_tvWrite_clicked(const QModelIndex &_index)
{


    UpToParentFiles(write_tree_model, _index, write_tree_model->itemFromIndex(_index)->checkState());
    DownToChildrensFiles(write_tree_model, _index, write_tree_model->itemFromIndex(_index)->checkState(), Write);

    SaveDescriptionModel(ui->pteDesRead_2->toPlainText());

    QStandardItem *item=write_tree_model->itemFromIndex(_index);
    if(item!=NULL)
    {
        qlonglong d=item->data(Qt::UserRole+1).toLongLong();

        if(d>0 || d==-2)
        {
            _current_local_num=item->data(Qt::UserRole+2).toLongLong();
            _current_server_num=item->data(Qt::UserRole+3).toLongLong();
        }
        else
        {
            _current_local_num=-1;
            _current_server_num=-1;
        }

        ui->pbLoadPreviews->setEnabled( ui->rbSourceServ->isChecked() && (d>0 || d==-2));
        ui->pbLoadPreviews_2->setEnabled( ui->rbSourceServ_2->isChecked() && (d>0 || d==-2));

        DisplayInfo(_current_local_num, _current_server_num);
    }
    else
    {
        DisplayInfo(0,0);
    }
}
//----------------------------------------------------------
void MainForm::on_pbWrite_clicked()
{
    SaveDescriptionModel(ui->pteDesRead_2->toPlainText());
    max_models=0;

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
        max_models=db_op->GetCountSendDelModels();

        int x=this->geometry().x();
        int y=this->geometry().y();
        int w=this->geometry().width();
        int h=this->geometry().height();
        emit ProgressStart(max_models, x, y, w, h);

        mod_conv->StartSendDeleteFiles(max_models);
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
void MainForm::ShowContextMenu(const QPoint _pos, const bool _read)
{
    QPoint global_pos;
    if(sender()->inherits("QAbstractScrollArea"))
    {
        global_pos=((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(_pos);
    }
    else
    {
        if(_read)
        {
            global_pos=ui->tvRead->mapToGlobal(_pos);
        }
        else
        {
            global_pos=ui->tvWrite->mapToGlobal(_pos);
        }
    }

    QModelIndex click_index;
    if(_read)
    {
        click_index=ui->tvRead->indexAt(_pos);
    }
    else
    {
        click_index=ui->tvWrite->indexAt(_pos);
    }

    bool valid=click_index.isValid();

    if(valid)
    {

        QStandardItem *item;
        if(_read)
        {
            item=read_tree_model->itemFromIndex(click_index);
        }
        else
        {
            item=write_tree_model->itemFromIndex(click_index);
        }

        QString txt=item->text();
        qlonglong d=item->data(Qt::UserRole+1).toLongLong();
        QMenu menu;
        qlonglong local_num=item->data(Qt::UserRole+2).toLongLong();
        qlonglong server_num=item->data(Qt::UserRole+3).toLongLong();
        if(d==-2)
        {


            if(local_num!=0)
            {
                QAction *action1=new QAction(QString::fromUtf8("Принять актуальной локальную версию"), this);
                action1->setData(1);
                menu.addAction(action1);
            }

            if(server_num!=0)
            {
                QAction *action2=new QAction(QString::fromUtf8("Принять актуальной серверную версию"), this);
                action2->setData(2);
                menu.addAction(action2);
            }
        }
        if((d>0 && d!=4 || d==-2) && server_num!=0)
        {
            QAction *action3=new QAction(QString::fromUtf8("Скопировать модель с сервера для анализа"), this);
            action3->setData(3);
            menu.addAction(action3);
        }

        if(menu.actions().size()>0)
        {
        QAction* selectedItem=menu.exec(global_pos);


        if(selectedItem)
        {
            int menu_id=selectedItem->data().toInt();

            switch(menu_id)
            {
            case 1:
            {
                db_op->ActualiseModel(_user_login, server_num, true);
                BuildingTree(_user_login);
                ConstructTree(Read, list_compare);
                ConstructTree(Write, list_compare);

                RecoveryTreeIndex();
                break;
            }
            case 2:
            {
                db_op->ActualiseModel(_user_login, local_num, false);

                BuildingTree(_user_login);
                ConstructTree(Read, list_compare);
                ConstructTree(Write, list_compare);

                RecoveryTreeIndex();
                break;
            }
            case 3:
            {
                QString Analys_folder=QFileDialog::getExistingDirectory(0, QString::fromUtf8(QString("Укажи куда скопировать модель").toAscii()),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
                if(Analys_folder!="")
                {
                    QString mess=db_op->VerifyCustomCopyPath(Analys_folder);
                    if(mess=="")
                    {
                    StartReadModeles(Analys_folder+"/", server_num, false);
                    }
                    else
                    {
                        QMessageBox MB;
                        MB.setText(mess);
                        MB.setWindowTitle(QString("Ошибка"));
                        MB.exec();
                    }
                }
                break;
            }

            }
        }
}

    }
}

//----------------------------------------------------------
void MainForm::InternalCallBuildingTree()
{
    if(!GetIsTransaction())
    {
        BuildingTree(_user_login);
        ConstructTree(Read, list_compare);
        ConstructTree(Write, list_compare);
    }
}
//----------------------------------------------------------
void MainForm::StartReadModeles(const QString &_root, const qlonglong _server_num_model, const bool _is_preview)
{
    max_models=-1;
    QString destination=_root;
    mod_conv->SetTransactionFlag(true);
    mod_conv->Clear();
    int is_work=false;
    int custom_copy=0;
    if(_server_num_model==0)
    {

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
    }
    else
    {
        QStringList files_list;
        if(_is_preview)
        {
            db_op->GetServerListPreviews(_server_num_model, files_list);
            custom_copy=2;
            destination=_root;
        }
        else
        {
            db_op->GetServerListFiles(_server_num_model, files_list);
            custom_copy=1;
            destination=_root;
        }
        max_models=1;
        for(int i=0; i<files_list.size(); i++)
        {
            mod_conv->ReceiveFile(files_list[i]);
        }

        is_work=true;

    }
    if(is_work)
    {
        if(max_models<0)
        {
            max_models=db_op->GetCountRecDelModels();
        }
        int x=this->geometry().x();
        int y=this->geometry().y();
        int w=this->geometry().width();
        int h=this->geometry().height();

        emit ProgressStart(max_models, x, y, w, h);

        mod_conv->StartReceiveDeleteFiles(destination, custom_copy, max_models);
    }
    else
    {
        QMessageBox MB;
        MB.setText(QString::fromUtf8("Нет выделеных моделей для чтения"));
        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
        MB.exec();
    }
}


//----------------------------------------------------------
void MainForm::on_tvRead_customContextMenuRequested(const QPoint &_pos)
{
    ShowContextMenu(_pos, true);
}
//----------------------------------------------------------
void MainForm::on_tvWrite_customContextMenuRequested(const QPoint &_pos)
{
    ShowContextMenu(_pos, false);
}
//----------------------------------------------------------
void MainForm::ViewError(const int _num_error, const QString& _error, const QString &_detail, const QString& _client_detail)
{
    //Вообще будет выводиться куда-то на панель а пока так
    QMessageBox MB;
    MB.setText(_error+"\n"+_detail+"\n"+_client_detail);
    MB.setWindowTitle(QString::fromUtf8("Ошибка ")+QString::number(_num_error));
    MB.exec();
}
//----------------------------------------------------------
void MainForm::DisplayModelInfo(const qlonglong _loc_num, const qlonglong _serv_num)
{
    //мне понадобятся:
    //Название модели
    //Description
    //Список файлов (структура Название, размер, дата модификации, кто модифицировал)
    //список путей к превью


    current_local_model=_loc_num;
    QString title_model="";
    QString description="";
    QList<tFile> files_model;

    if(ui->rbSourseLoc->isChecked())
    {
        //локальные данные
        if(_loc_num!=0)
        {
            ui->pteDesRead->setReadOnly(false);
            ui->pteDesRead_2->setReadOnly(false);
        }
        else
        {
            ui->pteDesRead->setReadOnly(true);
            ui->pteDesRead_2->setReadOnly(true);
        }

        db_op->GetModelInfo(_loc_num, title_model, description, files_model, previews);

        ui->leModelTitle->setText(QString("Модель: ")+title_model);
        ui->leModelTitle_2->setText(QString("Модель: ")+title_model);
        ui->pteDesRead->setPlainText(description);
        ui->pteDesRead_2->setPlainText(description);


        delete table_files_model;
        table_files_model= new QStandardItemModel(files_model.size(),3,this);
        for(int i=0; i<files_model.size();i++)
        {
            QModelIndex i_col_name=table_files_model->index(i, 0, QModelIndex());
            QModelIndex i_col_size=table_files_model->index(i, 1, QModelIndex());
            QModelIndex i_col_last_mod=table_files_model->index(i, 2, QModelIndex());

            table_files_model->setData(i_col_name, files_model[i].file_name);

            table_files_model->setData(i_col_size, QString::number(files_model[i].size)+" b");

            table_files_model->setData(i_col_last_mod, files_model[i].last_mod);

            table_files_model->item(i,0)->setEditable(false);
            table_files_model->item(i,1)->setEditable(false);
            table_files_model->item(i,2)->setEditable(false);
        }
        QStringList header;
        header << QString("Имя файла") << QString("Размер") << QString("Изменен");
        table_files_model->setHorizontalHeaderLabels(header);

        ui->tabvListFiles->setModel(table_files_model);
        ui->tabvListFiles->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.3);
        ui->tabvListFiles->setSelectionBehavior(QAbstractItemView::SelectRows);

        ui->tabvListFiles_2->setModel(table_files_model);
        ui->tabvListFiles_2->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles_2->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles_2->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.3);
        ui->tabvListFiles_2->setSelectionBehavior(QAbstractItemView::SelectRows);

        if(previews.size()>0)
        {
            //есть превью
            num_preview=0;
            ui->tbFirstPreview->setEnabled(true);
            ui->tbPriorPreview->setEnabled(true);
            ui->tbNextPreview->setEnabled(true);
            ui->tbLastPreview->setEnabled(true);
            ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview->setEnabled(true);

            ui->tbFirstPreview_2->setEnabled(true);
            ui->tbPriorPreview_2->setEnabled(true);
            ui->tbNextPreview_2->setEnabled(true);
            ui->tbLastPreview_2->setEnabled(true);
            ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview_2->setEnabled(true);

            DisplayPreview(previews);
        }
        else
        {
            //нет превью
            ui->tbFirstPreview->setEnabled(false);
            ui->tbPriorPreview->setEnabled(false);
            ui->tbNextPreview->setEnabled(false);
            ui->tbLastPreview->setEnabled(false);
            ui->leNumPreview->setText("");
            ui->leNumPreview->setEnabled(false);

            ui->tbFirstPreview_2->setEnabled(false);
            ui->tbPriorPreview_2->setEnabled(false);
            ui->tbNextPreview_2->setEnabled(false);
            ui->tbLastPreview_2->setEnabled(false);
            ui->leNumPreview_2->setText("");
            ui->leNumPreview_2->setEnabled(false);

            DisplayPreview(previews);
        }
    }
    else
    {
        //серверные данные
        ui->pteDesRead->setReadOnly(true);
        ui->pteDesRead_2->setReadOnly(true);

        db_op->GetServerModelInfo(_serv_num, title_model, description, files_model);

        ui->leModelTitle->setText(QString("Модель: ")+title_model);
        ui->pteDesRead->setPlainText(description);

        ui->leModelTitle_2->setText(QString("Модель: ")+title_model);
        ui->pteDesRead_2->setPlainText(description);


        delete table_files_model;
        table_files_model= new QStandardItemModel(files_model.size(),4,this);
        for(int i=0; i<files_model.size();i++)
        {
            QModelIndex i_col_name=table_files_model->index(i, 0, QModelIndex());
            QModelIndex i_col_size=table_files_model->index(i, 1, QModelIndex());
            QModelIndex i_col_last_mod=table_files_model->index(i, 2, QModelIndex());
            QModelIndex i_col_who=table_files_model->index(i, 3, QModelIndex());

            table_files_model->setData(i_col_name, files_model[i].file_name);

            table_files_model->setData(i_col_size, QString::number(files_model[i].size)+" b");

            table_files_model->setData(i_col_last_mod, files_model[i].last_mod);

            table_files_model->setData(i_col_who, db_op->LoginFromNum(files_model[i].NumLoginMod));

            table_files_model->item(i,0)->setEditable(false);
            table_files_model->item(i,1)->setEditable(false);
            table_files_model->item(i,2)->setEditable(false);
            table_files_model->item(i,3)->setEditable(false);
        }
        QStringList header;
        header << QString("Имя файла") << QString("Размер") << QString("Изменен") << QString("Кем изменен");
        table_files_model->setHorizontalHeaderLabels(header);

        ui->tabvListFiles->setModel(table_files_model);
        ui->tabvListFiles->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles->setColumnWidth(3,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles->setSelectionBehavior(QAbstractItemView::SelectRows);

        ui->tabvListFiles_2->setModel(table_files_model);
        ui->tabvListFiles_2->setColumnWidth(0,(ui->tabvListFiles->width()-20)*0.5);
        ui->tabvListFiles_2->setColumnWidth(1,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles_2->setColumnWidth(2,(ui->tabvListFiles->width()-20)*0.2);
        ui->tabvListFiles_2->setColumnWidth(3,(ui->tabvListFiles->width()-20)*0.15);
        ui->tabvListFiles_2->setSelectionBehavior(QAbstractItemView::SelectRows);

        GetLoadedPreview(_serv_num, previews);

        if(previews.size()==0)
        {
            ui->tbFirstPreview->setEnabled(false);
            ui->tbPriorPreview->setEnabled(false);
            ui->tbNextPreview->setEnabled(false);
            ui->tbLastPreview->setEnabled(false);
            ui->leNumPreview->setText("");
            ui->leNumPreview->setEnabled(false);

            ui->tbFirstPreview_2->setEnabled(false);
            ui->tbPriorPreview_2->setEnabled(false);
            ui->tbNextPreview_2->setEnabled(false);
            ui->tbLastPreview_2->setEnabled(false);
            ui->leNumPreview_2->setText("");
            ui->leNumPreview_2->setEnabled(false);
        }
        else
        {
            num_preview=0;
            ui->tbFirstPreview->setEnabled(true);
            ui->tbPriorPreview->setEnabled(true);
            ui->tbNextPreview->setEnabled(true);
            ui->tbLastPreview->setEnabled(true);
            ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview->setEnabled(true);

            ui->tbFirstPreview_2->setEnabled(true);
            ui->tbPriorPreview_2->setEnabled(true);
            ui->tbNextPreview_2->setEnabled(true);
            ui->tbLastPreview_2->setEnabled(true);
            ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
            ui->leNumPreview_2->setEnabled(true);
        }
        DisplayPreview(previews);
    }
}
//----------------------------------------------------------
void MainForm::GetLoadedPreview(const qlonglong _serv_num, QStringList &_previews)
{
    _previews.clear();


    QString path=my_settings.GetTemp()+"Previews/"+QString::number(_serv_num)+"/"+db_op->GetServerModelPath(_serv_num)+"/";
    QDir dir(path);
    QStringList all_files=dir.entryList(QDir::Files | QDir::Hidden);
    for(int i=0; i<all_files.size(); i++)
    {
        QFileInfo fi(all_files[i]);
        QString suffix=fi.suffix();
        if(suffix.toLower()=="bmp" | suffix.toLower()=="jpg" | suffix.toLower()=="jpeg" | suffix.toLower()=="gif" | suffix.toLower()=="png")
        {
            _previews.push_back(path+all_files[i]);
        }
    }
}

//----------------------------------------------------------
void MainForm::DisplayPreview(const QStringList &_previews)
{
    if(_previews.size()==0)
    {
        Image = QImage();
    }
    else
    {
        Image = QImage(_previews[num_preview]);
    }

    int view_width=ui->graphicsView->width()-10;
    int view_height=ui->graphicsView->height()-10;

    QGraphicsScene *scene = new QGraphicsScene(this);

    scene->addPixmap(QPixmap::fromImage(Image));

    ui->graphicsView->setScene(scene);
    ui->graphicsView_2->setScene(scene);

    qreal dx=((qreal)view_width)/((qreal)Image.width());
    qreal dy=((qreal)view_height)/((qreal)Image.height());

    qreal res=0;

    if(dx>dy)
    {
        res=dy;
    }
    else
    {
        res=dx;
    }
    ui->graphicsView->resetMatrix();
    ui->graphicsView->scale(res,res);

    ui->graphicsView_2->resetMatrix();
    ui->graphicsView_2->scale(res,res);
}
//----------------------------------------------------------
void MainForm::on_tbFirstPreview_clicked()
{
    num_preview=0;
    ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    DisplayPreview(previews);
}
//----------------------------------------------------------
void MainForm::on_tbPriorPreview_clicked()
{
    num_preview--;
    if(num_preview<0)
    {
        num_preview=0;
    }
    ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    DisplayPreview(previews);
}
//----------------------------------------------------------
void MainForm::on_tbNextPreview_clicked()
{
    num_preview++;
    if(num_preview>previews.size()-1)
    {
        num_preview=previews.size()-1;
    }
    ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    DisplayPreview(previews);
}
//----------------------------------------------------------

void MainForm::on_tbLastPreview_clicked()
{
    num_preview=previews.size()-1;
    ui->leNumPreview->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    ui->leNumPreview_2->setText(QString::number(num_preview+1)+" из "+QString::number(previews.size()));
    DisplayPreview(previews);
}
//----------------------------------------------------------
void MainForm::on_pteDesRead_textChanged()
{

    if(ui->tabWidget->currentIndex()==0)
    {
        QString text=ui->pteDesRead->toPlainText();

        ui->pteDesRead_2->setPlainText(text);
    }
}
//----------------------------------------------------------
void MainForm::on_pteDesRead_2_textChanged()
{
    if(ui->tabWidget->currentIndex()==1)
    {
        QString text=ui->pteDesRead_2->toPlainText();

        ui->pteDesRead->setPlainText(text);
    }
}
//----------------------------------------------------------
void MainForm::SaveDescriptionModel(const QString &_text)
{
    QString file_name="";
    QString hash="";
    QDateTime last_mod;
    qint64 size=0;

    //Записать изменения в базу, определить имя файла информации о модели
    db_op->SaveDescription(current_local_model, _text, file_name);

    //Сверить, в самом ли деле отличается то что в файле с тем что в окне
    //Если отличается - записать, пересчитать хэш и узнать время модификации
    EditingModelFile(file_name, _text, hash, last_mod, size);

    //если файл менялся то хэш не будет пустым

    if(hash!="")
    {
        //и нужно переписать хэш самого файла в таблице локальных файлов
        //а также дату модификации
        //после этого пересчитать общий хэш модели, и исправить общий хэш в локальной таблице моделей
        db_op->UpdateInfoData(current_local_model, file_name, hash, last_mod);

        EndUpdateServerModel(false);
    }
}

//----------------------------------------------------------
void MainForm::EditingModelFile(const QString &_file_name, const QString& _text, QString& _hash, QDateTime &_dt, qint64 &_size)
{

    if(_file_name!="")
    {
        QSettings s(my_settings.GetRoot()+_file_name, QSettings::IniFormat);
        QTextCodec *codec =QTextCodec::codecForName("UTF-8");
        s.setIniCodec(codec);

        QString cur_text=s.value("Description", "").toString();
        if(cur_text!=_text)
        {
            s.setValue("Description", _text);

            //Найти новые дату модификации и хэш файла
            QFileInfo fi(my_settings.GetRoot()+_file_name);
            _dt=fi.lastModified();
            tCalcHash ch;
            ch.GetFileHash(my_settings.GetRoot()+_file_name);
            _hash=ch.ResultHash();
            _size=fi.size();
        }
    }
}
//----------------------------------------------------------
void MainForm::on_rbSourseLoc_clicked()
{
    ui->rbSourseLoc_2->setChecked(ui->rbSourseLoc->isChecked());
    ui->pbLoadPreviews->setEnabled(false);
    ui->pbLoadPreviews_2->setEnabled(false);

    DisplayInfo(_current_local_num, _current_server_num);
}
//----------------------------------------------------------
void MainForm::on_rbSourceServ_clicked()
{
    ui->rbSourceServ_2->setChecked(ui->rbSourceServ->isChecked());
    if(_current_local_num>0 || _current_server_num>0)
    {
        ui->pbLoadPreviews->setEnabled(true);
        ui->pbLoadPreviews_2->setEnabled(true);

        DisplayInfo(_current_local_num, _current_server_num);
    }



}
//----------------------------------------------------------
void  MainForm::DisplayInfo(const qlonglong _loc_num, const qlonglong _serv_num)
{


    if(_loc_num>0 || _serv_num>0)
    {

        DisplayModelInfo(_loc_num, _serv_num);
    }
    else
    {
        DisplayModelInfo(0, 0);
    }

}
//----------------------------------------------------------
void MainForm::on_pbLoadPreviews_clicked()
{
    //Загрузка превью с сервера

    if(_current_local_num>0 || _current_server_num>0)
    {
        QString prev_path=my_settings.GetTemp()+"Previews/"+QString::number(_current_server_num)+"/";
        QDir dir;
        dir.setPath(prev_path);
        removeFolder(dir, true);

        StartReadModeles(prev_path, _current_server_num, true);
    }

}
//----------------------------------------------------------
bool MainForm::removeFolder(const QDir & _dir, const bool _del_dir)
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
void MainForm::on_tbFirstPreview_2_clicked()
{
    on_tbFirstPreview_clicked();
}
//----------------------------------------------------------
void MainForm::on_tbPriorPreview_2_clicked()
{
    on_tbPriorPreview_clicked();
}
//----------------------------------------------------------
void MainForm::on_tbNextPreview_2_clicked()
{
    on_tbNextPreview_clicked();
}
//----------------------------------------------------------
void MainForm::on_tbLastPreview_2_clicked()
{
    on_tbLastPreview_clicked();
}
//----------------------------------------------------------
void MainForm::on_pbLoadPreviews_2_clicked()
{
    on_pbLoadPreviews_clicked();
}
//----------------------------------------------------------
void MainForm::on_rbSourseLoc_2_clicked()
{
    ui->rbSourseLoc->setChecked(ui->rbSourseLoc_2->isChecked());
    on_rbSourseLoc_clicked();
}
//----------------------------------------------------------
void MainForm::on_rbSourceServ_2_clicked()
{
    ui->rbSourceServ->setChecked(ui->rbSourceServ_2->isChecked());
    on_rbSourceServ_clicked();
}
//----------------------------------------------------------
void MainForm::RecoveryTreeIndex()
{
    QModelIndex ReadIndex=QModelIndex();
    QModelIndex WriteIndex=QModelIndex();

    SearchIndex(_current_local_num, _current_server_num, ReadIndex, WriteIndex);

    ui->tvRead->setCurrentIndex(ReadIndex);
    ui->tvWrite->setCurrentIndex(WriteIndex);

    if(ui->tabWidget->currentIndex()==0)
    {
        if(ReadIndex==QModelIndex())
        {
            DisplayModelInfo(0,0);
        }
        else
        {
            DisplayModelInfo(_current_local_num, _current_server_num);
        }
    }
    else
    {
        if(WriteIndex==QModelIndex())
        {
            DisplayModelInfo(0,0);
        }
        else
        {
            DisplayModelInfo(_current_local_num, _current_server_num);
        }
    }

}

//----------------------------------------------------------
void MainForm::on_lvLogins_customContextMenuRequested(const QPoint &_pos)
{
    QPoint global_pos;
    if(sender()->inherits("QAbstractScrollArea"))
    {
        global_pos=((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(_pos);
    }
    else
    {

            global_pos=ui->lvLogins->mapToGlobal(_pos);

    }

    QModelIndex click_index;
    click_index=ui->lvLogins->indexAt(_pos);

        QMenu menu;



    QAction *action1=new QAction(QString::fromUtf8("Добавить пользователя"), this);
    action1->setData(1);
    menu.addAction(action1);

    if(click_index!=QModelIndex())
    {
    QAction *action2=new QAction(QString::fromUtf8("Отредактировать пользователя"), this);
    action2->setData(2);
    menu.addAction(action2);

    QAction *action3=new QAction(QString::fromUtf8("Удалить пользователя"), this);
    action3->setData(3);
    menu.addAction(action3);
    }
    QAction* selectedItem=menu.exec(global_pos);

    if(selectedItem)
    {
        int menu_id=selectedItem->data().toInt();

        switch(menu_id)
        {
        case 1:
        {
            OnNewLogin();
            break;
        }
        case 2:
        {
            OnEditLogin();
            break;
        }
        case 3:
        {
            OnDelLogin();
            break;
        }
        }
    }

}
