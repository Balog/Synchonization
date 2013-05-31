#include "tModelsConveyor.h"
#include <QMessageBox>

tModelsConveyor::tModelsConveyor(Ui::MainForm *_ui, QObject* _link, tDatabaseOp *_db_op, QObject *parent) :
    QObject(parent), ui(_ui), db_op(_db_op), link(_link),Transaction(false), user_login("")
{

    mod_custom_copy=false;
    conv=new tConveyor(ui, link, db_op, 0);


    connect(_link, SIGNAL(NextCommand()), conv, SLOT(NextCommand()));
    connect(conv, SIGNAL(EndCommands()), this, SLOT(EndConveyor()));

    connect(_link, SIGNAL(RunGui(QByteArray&)), conv, SLOT(OnRunGuiCommand(QByteArray&)));
    connect(conv, SIGNAL(Disconnect()), _link, SLOT(OnDisconnect()));
    connect(conv, SIGNAL(CloseMain()), _link, SLOT(close()));
    connect(conv, SIGNAL(AutorizStart()), _link, SLOT(OnAutorizStart()));
    connect(conv, SIGNAL(SetVisible(bool)),_link, SLOT(OnSetVisible(bool)));
    connect(_link, SIGNAL(Disconnecting()), conv, SLOT(OnDisconnecting()));

    connect(conv, SIGNAL(EndTransactions()), _link, SLOT(EndTransactions()));
    connect(this, SIGNAL(EndTransactions()), _link, SLOT(EndTransactions()));
    connect(conv, SIGNAL(EndConveyor()), this, SLOT(EndConveyor()));



}
//-------------------------------------------------------------------------
tModelsConveyor::~tModelsConveyor()
{
    delete conv;
    conv=NULL;
}

//-------------------------------------------------------------------------
void tModelsConveyor::StartServer(const QString &_addr, const int _port)
{
    conv->StartServer(_addr, _port);
}
//-------------------------------------------------------------------------
void tModelsConveyor::Clear()
{
    conv->Clear();
    db_op->ClearlModConv();

}
//-------------------------------------------------------------------------
void tModelsConveyor::SendFile(const QString &_file_name)
{
    db_op->PrepareSendFile(_file_name);

}
//-------------------------------------------------------------------------
void tModelsConveyor::DeletingServerFile(const QString &_file_name)
{
    db_op->PrepareDeletingServer(_file_name);
}
//-------------------------------------------------------------------------
void tModelsConveyor::CancelOperations()
{
    conv->CancelOperations();
}
//-------------------------------------------------------------------------
void tModelsConveyor::EndConveyor()
{
    conv->ClearTempFolder();
    conv->Clear();
    if(send)
    {
        StartSendDeleteFiles(max_model);
    }
    else
    {
        StartReceiveDeleteFiles(root_folder, mod_custom_copy, max_model);
    }


}
//-------------------------------------------------------------------------
void tModelsConveyor::OnDisconnect()
{
    emit Disconnecting();
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnAutorizStart()
{
    emit AutorizStart();
}
//-------------------------------------------------------------------------
void tModelsConveyor::ErrorConveyor()
{
    qDebug() << QString::fromUtf8("Выполнение пакета команд прервано");
    l="tModelsConveyor \tErrorConveyor\tВыполнение пакета команд прервано";
    log.Write(l);

    EndConveyor();
}
//-------------------------------------------------------------------------
void tModelsConveyor::Autorization(const QString& _login, const QString& _password)
{
    l="tModelsConveyor \tAutorization\tОтправка команды авторизации";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SendAutorization");
    out << tr("SendAutorization");
    out << _login;
    out << _password;

    emit RunGui(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnListFiles()
{
    l="tModelsConveyor \t OnListFiles \t Отправка команды запроса списка файлов из конвейера моделей";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GetListModels");
    out << tr("GetListModels");

    emit RunGui(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::ReceiveFile(const QString& _file_name)
{
    db_op->PrepareReceiveFile(_file_name);
}
//-------------------------------------------------------------------------
void tModelsConveyor::DeletingLocalFile(const QString& _file_name)
{
    db_op->PrepareDeletingLocal(_file_name);
}
//-------------------------------------------------------------------------
void tModelsConveyor::StartSendDeleteFiles(const int _max_model)
{
    l="tModelsConveyor \tStartSendDeleteFiles\tФормирование списка команд транзакции передачи и удаления файлов на сервере";
    log.Write(l);

    send=true;
    //    Transaction=true;
    bool stop=false;
    QString name_model="";
    max_model=_max_model;
    count_models=max_model;
    if(db_op->GetNextSendDelModel(name_model, count_models))
    {
        emit SignalCountFiles(count_models);
        //получено имя очередной модели, некоторые файлы которой должны быть отправлены на сервер
        QStringList SendModelFiles;
        db_op->GetSendModelFiles(name_model, SendModelFiles);

        if(SendModelFiles.size()!=0)
        {
            //Если есть файлы для записи на сервер
            //Внести данные о пересылаемых файлах в список пересылаемыз файлов
            for(int i=0; i<SendModelFiles.size(); i++)
            {
                QString S=SendModelFiles[i];
                stop=conv->SendFile(S, all_files);
                if(stop)
                {
                    break;
                }
            }
        }

        QStringList DeleteServerModelFiles;
        db_op->GetDeleteServerModelFiles(name_model, DeleteServerModelFiles);

        if(DeleteServerModelFiles.size()!=0)
        {
            for(int i=0; i<DeleteServerModelFiles.size(); i++)
            {
                QString S=DeleteServerModelFiles[i];
                stop=conv->DeletingFile(S, all_files, send);
            }
        }


        conv->AddStartTransaction(send);

        if(SendModelFiles.size()!=0)
        {
            stop=conv->AddSendCommand();
        }

        if(DeleteServerModelFiles.size()!=0)
        {
            conv->AddDelCommand();
        }

        conv->AddCommitTransaction(send, QString(""), false);


        //Начало выполнения списка команд
        if(!stop)
        {
            conv->StartExecution();
        }
        else
        {

            qDebug() << QString::fromUtf8("Локальный файл изменился. Операция прервана. ");

            l="tModelsConveyor \tStartSendDeleteFiles\tЛокальный файл изменился. Выполнение списка отменено";
            log.Write(l);

            EndConveyor();
        }
    }
    else
    {
        l="tModelsConveyor \tStartSendDeleteFiles\tВсе модели отправлены";
        log.Write(l);


        //СЮДА НАЧАЛО ПРОЦЕДУРЫ ОБНОВЛЕНИЯ LAST
        //ДО ОБНОВЛЕНИЯ СЕРВЕРНЫХ ТАБЛИЦ (ОКОНЧАНИЕ В void tReportGuiGetListServerModels::ExeCommand)

        l="tModelsConveyor \tStartSendDeleteFiles\t НАЧАЛО ПРОЦЕДУРЫ ОБНОВЛЕНИЯ LAST";
        log.Write(l);

        emit SignalCountFiles(count_models);

        db_op->PrepareUpdateLastSynch(true, user_login);

        MarkLastTables(true, user_login);
        conv->GetServerModels();
    }

}
//-------------------------------------------------------------------------
void tModelsConveyor::MarkLastTables(const bool _send, const QString& _user_login)
{
    for(int i=0; i<all_files.size(); i++)
    {
        db_op->UpdateLastSynchMark(all_files[i], _send, _user_login);
    }
}

//-------------------------------------------------------------------------
void tModelsConveyor::StartReceiveDeleteFiles(const QString &_root, int _custom_copy, int _max_model)
{
    mod_custom_copy=_custom_copy;
    l="tModelsConveyor \tStartReceiveDeleteFiles\tФормирование списка команд транзакции приема и удаления файлов на клиенте";
    log.Write(l);
    root_folder=_root;

    conv->ClearTempFolder();
    send=false;

    bool stop=false;
    QString name_model="";
    max_model=_max_model;
    count_models=max_model;

    if(db_op->GetNextReceiveDelModel(name_model, count_models))
    {
        emit SignalCountFiles(count_models);
        //получено имя очередной модели что нужно получить с сервера
        QStringList ReceiveModelFiles;
        db_op->GetReceiveModelFiles(name_model, ReceiveModelFiles);
        if(ReceiveModelFiles.size()!=0)
        {
            //Если есть файлы для чтения с сервера
            //Внести данные о получаемых файлах в список получаемых файлов
            for(int i=0; i<ReceiveModelFiles.size(); i++)
            {
                QString S=ReceiveModelFiles[i];
                stop=conv->ReceiveFile(S, all_files);
                if(stop)
                {
                    break;
                }
            }
        }

        QStringList DeleteLocalModelFiles;
        db_op->GetDeleteLocalModelFiles(name_model, DeleteLocalModelFiles);

        if(DeleteLocalModelFiles.size()!=0)
        {
            for(int i=0; i<DeleteLocalModelFiles.size(); i++)
            {
                QString S=DeleteLocalModelFiles[i];
                stop=conv->DeletingFile(S, all_files, send);
            }
        }
        conv->AddStartTransaction(send);
        stop=conv->AddReceiveCommand(_root);
        //        conv->AddDelCommand();

        conv->AddCommitTransaction(send, root_folder, _custom_copy);

        //Начало выполнения списка команд
        if(!stop)
        {
            conv->StartExecution();
        }
        else
        {

            qDebug() << QString::fromUtf8("Локальный файл изменился. Операция прервана. ");

            l="tModelsConveyor \tStartReceiveDeleteFiles\tЛокальный файл изменился. Выполнение списка отменено";
            log.Write(l);

            EndConveyor();
        }
    }
    else
    {
        l="tModelsConveyor \tStartReceiveDeleteFiles\tВсе модели приняты";
        log.Write(l);

        if(_custom_copy==0)
        {
        //нужна команда GetListModels

        db_op->RefreshModelsFiles();

        MarkLastTables(false, user_login);

        db_op->ExecUpdateLastSynch(false, user_login);

        ClearAllList();
        }
        emit SignalCountFiles(max_model);
        emit EndTransactions();
    }

}
//-------------------------------------------------------------------------
void tModelsConveyor::CorrectLastSynch(const bool _server)
{
    conv->CorrectLastSynch(all_files, _server);
    all_files.clear();
}
//-------------------------------------------------------------------------
void tModelsConveyor::SetTransactionFlag(const bool _flag)
{
    Transaction=_flag;
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnEndTransactions()
{
    db_op->RefreshModelsFiles();
    emit EndTransactionsMain();
}
//-------------------------------------------------------------------------
void tModelsConveyor::SendLoginPassword(const QString &_login, const QString &_password, const int _row, const bool _new_user)
{
    l="tModelsConveyor \tSendLoginPassword\tПроверка и регистрация пользователя на сервере";
    log.Write(l);

    qlonglong num_log=0;
    if(!_new_user)
    {
        num_log=db_op->GetNumLogin(_row);
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SaveLoginPassword");
    out << tr("SaveLoginPassword");
    out << _login;
    out << _password;
    out << _new_user;
    out << num_log;
    out << _row;

    conv->OnRunGuiCommand(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::SendDeleteLogin(const int _row)
{
    l="tModelsConveyor \tSendDeleteLogin\t Удаление пользователя с сервера";
    log.Write(l);

    qlonglong num_log=db_op->GetNumLogin(_row);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIDeleteLogin");
    out << tr("DeleteLogin");
    out << num_log;

    conv->OnRunGuiCommand(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::ReceiveLoginsTable()
{
    l="tModelsConveyor \tReceiveLoginsTable\t Запрос таблицы логинов";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReceiveLoginsTable");
    out << tr("ReceiveLoginsTable");

    conv->OnRunGuiCommand(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::SavePermissionsToServer(const qlonglong _num_login)
{
    l="tModelsConveyor \tSavePermissionsToServer\t Запись на сервер таблицы разрешений";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUISavePermissions");
    out << tr("SavePermissions");

    db_op->SavePermissionsToServer(_num_login, block);

    conv->OnRunGuiCommand(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::SetLogin(const QString& _user_login)
{
    user_login=_user_login;
    conv->SetLogin(_user_login);
}
//-------------------------------------------------------------------------
void tModelsConveyor::ClearAllList()
{
    all_files.clear();

}
//-------------------------------------------------------------------------
void tModelsConveyor::SaveLoginWritable(const QStandardItemModel* _model, const int _row)
{
    bool check=false;
    db_op->SaveLoginsWritable(_model, _row, check);
    l="tModelsConveyor \tSaveLoginWritable\tЗапись разрешения на запись на сервере";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SaveLoginWritable");
    out << tr("SaveLoginWritable");
    out << _row;
    out << check;

    conv->OnRunGuiCommand(block);
}
