#include "tModelsConveyor.h"
#include <QMessageBox>

tModelsConveyor::tModelsConveyor(Ui::MainForm *_ui, QObject* _link, tDatabaseOp *_db_op, QObject *parent) :
    QObject(parent), ui(_ui), db_op(_db_op), link(_link),Transaction(false), user_login("")
{


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
        StartSendDeleteFiles();
    }
    else
    {
        StartReceiveDeleteFiles();
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
void tModelsConveyor::Autorization(QString& _login, QString& _password)
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
void tModelsConveyor::StartSendDeleteFiles()
{
    l="tModelsConveyor \tStartSendDeleteFiles\tФормирование списка команд транзакции передачи и удаления файлов на сервере";
    log.Write(l);

    send=true;
    //    Transaction=true;
    bool stop=false;
    QString name_model="";
    if(db_op->GetNextSendDelModel(name_model))
    {

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

        conv->AddCommitTransaction(send);


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

        db_op->PrepareUpdateLastSynch(true, user_login);

        MarkLastTables(true, user_login);
        conv->GetServerModels();
    }

}
//-------------------------------------------------------------------------
void tModelsConveyor::MarkLastTables(bool _send, const QString& user_login)
{
    for(int i=0; i<all_files.size(); i++)
    {
        db_op->UpdateLastSynchMark(all_files[i], _send, user_login);
    }
}

//-------------------------------------------------------------------------
void tModelsConveyor::StartReceiveDeleteFiles()
{
    l="tModelsConveyor \tStartReceiveDeleteFiles\tФормирование списка команд транзакции приема и удаления файлов на клиенте";
    log.Write(l);

    conv->ClearTempFolder();
    send=false;
    //    Transaction=true;
    bool stop=false;
    QString name_model="";
    if(db_op->GetNextReceiveDelModel(name_model))
    {
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
        stop=conv->AddReceiveCommand();
        //        conv->AddDelCommand();

        conv->AddCommitTransaction(send);

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

            //            conv->Clear();
            EndConveyor();
        }
    }
    else
    {
        l="tModelsConveyor \tStartReceiveDeleteFiles\tВсе модели приняты";
        log.Write(l);

        //нужна команда GetListModels

        db_op->RefreshModelsFiles();

        MarkLastTables(false, user_login);

        db_op->ExecUpdateLastSynch(false, user_login);

        ClearAllList();

        emit EndTransactions();
    }
}
//-------------------------------------------------------------------------
void tModelsConveyor::CorrectLastSynch(bool _server)
{
    conv->CorrectLastSynch(all_files, _server);
    all_files.clear();
}
//-------------------------------------------------------------------------
void tModelsConveyor::SetTransactionFlag(bool _flag)
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
void tModelsConveyor::SendLoginPassword(QString &_login, QString &_password, int _row, bool _new_user)
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
void tModelsConveyor::SendDeleteLogin(int _row)
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
void tModelsConveyor::SavePermissionsToServer(qlonglong _num_login)
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
void tModelsConveyor::SaveLoginWritable(QStandardItemModel* model, int _row)
{
    bool check=false;
    db_op->SaveLoginsWritable(model, _row, check);
    l="tModelsConveyor \tSaveLoginWritable\tЗапись разрешения на запись на сервере";
    log.Write(l);

//    qlonglong num_log=0;
//    if(!_new_user)
//    {
//        num_log=db_op->GetNumLogin(_row);
//    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SaveLoginWritable");
    out << tr("SaveLoginWritable");
    out << _row;
    out << check;

    conv->OnRunGuiCommand(block);
}
