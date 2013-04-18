#include "MainForm.h"
#include "ui_MainForm.h"
#include <QFileDialog>

extern tSettings my_settings;

MainForm::MainForm(QWidget *parent) : ui(new Ui::MainForm), QDialog(parent),mod_conv(NULL), db_op(NULL), sLM_loc_list_models(NULL), slm_server_list_models(NULL), slm_list(NULL)
{
    sLM_Send=new QStringListModel;
    sLM_Del=new QStringListModel;
    sLM_Rec=new QStringListModel;
    sLM_DelLoc=new QStringListModel;

//    QTextCodec *codec =QTextCodec::codecForName("Windows-1251");
    QTextCodec *codec =QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);


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


//    conv=new tConveyor(ui);
//    conv->SetLink(this);
    db_op=new tDatabaseOp();

    db_op->RefreshModelsFiles();

    mod_conv= new tModelsConveyor(ui, this, db_op);

//    connect(this, SIGNAL(NextCommand()), conv, SLOT(NextCommand()));
//    connect(conv, SIGNAL(EndCommands()), this, SLOT(EndConveyor()));

//    connect(this, SIGNAL(RunGui(QByteArray&)), mod_conv, SLOT(OnRunGuiCommand(QByteArray&)));
//    connect(mod_conv, SIGNAL(Disconnect()), this, SLOT(OnDisconnect()));
//    connect(mod_conv, SIGNAL(CloseMain()), this, SLOT(close()));
//    connect(mod_conv, SIGNAL(AutorizStart()), this, SLOT(OnAutorizStart()));
//    connect(mod_conv, SIGNAL(SetVisible(bool)),this, SLOT(OnSetVisible(bool)));
//    connect(this, SIGNAL(Disconnecting()), mod_conv, SLOT(OnDisconnecting()));
//    connect(mod_conv, SIGNAL(ErrorCommands()),this, SLOT(ErrorConveyor()));

    mod_conv->StartServer(ui->leAddr->text(), ui->sbPort->value());



//    conv->SetDatabaseOperator(db_op);
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

    delete mod_conv;
    mod_conv=NULL;

    delete db_op;
    db_op=NULL;

    delete ui;
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
//    NumCelServModel=ui->lwListModels->currentIndex().row();
//    OnListFiles();

//    OnListFilesLocal();

    QMessageBox MB;
    MB.setText(QString::fromUtf8("Пакет транзакций выполнен"));
    MB.setWindowTitle(QString::fromUtf8("Пакет транзакций"));
    MB.exec();
}
//---------------------------------------------------------------------
//void MainForm::ErrorConveyor()
//{
//    QMessageBox MB;
//    MB.setText(QString::fromUtf8("Выполнение пакета команд прервано"));
//    MB.setWindowTitle(QString::fromUtf8("Пакет команд"));
//    MB.exec();
//}
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

//    bool stop=false;
//    QModelIndex MI=ui->lwListModels->currentIndex();

//    int N=MI.row();
//    if(N<0)
//    {
//        QMessageBox MB;
//        MB.setText(QString::fromUtf8("Выделите серверную модель"));
//        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
//        MB.exec();
//    }
//    else
//    {
//    QStringListModel *M=new QStringListModel;
//    M=(QStringListModel *)MI.model();

//    QString S=M->stringList().value(N);

//    mod_conv->Clear();

//    //Обработка списка пересылаемых файлов

//    if(listSend.size()!=0)
//    {
//        //Если есть файлы для записи на сервер
//        //Внести данные о пересылаемых файлах в список пересылаемыз файлов
//        for(int i=0; i<listSend.size(); i++)
//        {
//            QString S=listSend[i];
//            stop=conv->SendFile(S);
//            if(stop)
//            {
//                break;
//            }
//        }
//    }

//    if(listDel.size()!=0)
//    {
//        for(int i=0; i<listDel.size(); i++)
//        {
//        QString S=listDel[i];
//        stop=conv->DeletingFile(S, true);
//        if(stop)
//        {
//            break;
//        }
//        }


//    }
//    conv->AddStartTransaction(true);
//    stop=conv->AddSendCommand();
//    conv->AddDelCommand();

//    conv->AddCommitTransaction(true, S);

//    //Начало выполнения списка команд
//    if(!stop)
//    {
//    conv->StartExecution();
//    }
//    else
//    {
//        QMessageBox MB;
//        MB.setText(QString::fromUtf8("Локальный файл изменился. Операция прервана."));
//        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
//        MB.exec();
//    }
//}
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
//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);

//    out << tr("GetListFiles");
//    out << tr("GetListFiles");

//    emit RunGui(block);

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
//    bool stop=false;
//    QModelIndex MI=ui->lwLocalListModels->currentIndex();

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
//    QStringListModel *M=new QStringListModel;
//    M=(QStringListModel *)MI.model();

//    QString S=M->stringList().value(N);

//    conv->Clear();

//    if(listRec.size()>0)
//    {
//        for(int i=0; i<listRec.size(); i++)
//        {
//            QString S=listRec[i];
//            stop=conv->ReceiveFile(S);
//            if(stop)
//            {
//                break;
//            }
//        }
//        conv->AddStartTransaction(false);

//        conv->AddReceiveCommand();

//        conv->AddCommitTransaction(false, S);


//    }
////    conv->Clear();

//    if(listDelLoc.size()>0)
//    {
//        for(int i=0;i<listDelLoc.size(); i++)
//        {
//            QString S=listDelLoc[i];
//            stop=conv->DeletingFile(S, false);
//            if(stop)
//            {
//                break;
//            }
//        }

//        conv->AddCommitTransactionDel();
//    }

//    if(!stop)
//    {
//    conv->StartExecution();
//    }
//    else
//    {
//        QMessageBox MB;
//        MB.setText(QString::fromUtf8("Локальный файл изменился. Операция прервана."));
//        MB.setWindowTitle(QString::fromUtf8("Ошибка"));
//        MB.exec();
//    }
//    }

}
//---------------------------------------------------------------------
void MainForm::OnListFilesLocal()
{
    QString root=my_settings.GetRoot();
    QStringList list;
//    CreateFileList(root, list);
    db_op->RefreshModelsFiles();//ВРЕМЕННО ПОКА НЕТ СВЯЗИ ПЕРЕДАЧ И БАЗЫ
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
