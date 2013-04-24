#include "tModelsConveyor.h"
#include <QMessageBox>

tModelsConveyor::tModelsConveyor(Ui::MainForm *_ui, QObject* _link, tDatabaseOp *_db_op, QObject *parent) :
    QObject(parent), ui(_ui), db_op(_db_op), link(_link),Transaction(false)
{
//    db_op=new tDatabaseOp();

//    db_op->RefreshModelsFiles();

    conv=new tConveyor(ui, link, db_op, 0);
//    conv->SetLink(link);

    connect(_link, SIGNAL(NextCommand()), conv, SLOT(NextCommand()));
    connect(conv, SIGNAL(EndCommands()), this, SLOT(EndConveyor()));

    connect(_link, SIGNAL(RunGui(QByteArray&)), conv, SLOT(OnRunGuiCommand(QByteArray&)));
    connect(conv, SIGNAL(Disconnect()), _link, SLOT(OnDisconnect()));
    connect(conv, SIGNAL(CloseMain()), _link, SLOT(close()));
    connect(conv, SIGNAL(AutorizStart()), _link, SLOT(OnAutorizStart()));
    connect(conv, SIGNAL(SetVisible(bool)),_link, SLOT(OnSetVisible(bool)));
    connect(_link, SIGNAL(Disconnecting()), conv, SLOT(OnDisconnecting()));
//    connect(conv, SIGNAL(ErrorCommands()),_link, SLOT(ErrorConveyor()));

    connect(conv, SIGNAL(EndTransactions()), _link, SLOT(EndTransactions()));



//    conv->SetDatabaseOperator(db_op);

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
//    QMessageBox MB;
//    MB.setText(QString::fromUtf8("Транзакция выполнена"));
//    MB.setWindowTitle(QString::fromUtf8("Транзакция"));
//    MB.exec();

    conv->Clear();
    StartSendDeleteFiles();
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
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Выполнение пакета команд прервано"));
    MB.setWindowTitle(QString::fromUtf8("Пакет команд"));
    MB.exec();

    conv->Clear();
    StartSendDeleteFiles();
}
//-------------------------------------------------------------------------
void tModelsConveyor::Autorization(QString& _login, QString& _password)
{
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
                stop=conv->SendFile(S);
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
                stop=conv->DeletingFile(S, true);
            }
        }


        conv->AddStartTransaction(true);
        stop=conv->AddSendCommand();
        conv->AddDelCommand();

        conv->AddCommitTransaction(true);


        //Начало выполнения списка команд
        if(!stop)
        {
            conv->StartExecution();
        }
        else
        {
            QMessageBox MB;
            MB.setText(QString::fromUtf8("Локальный файл изменился. Операция прервана."));
            MB.setWindowTitle(QString::fromUtf8("Ошибка"));
            MB.exec();

            conv->Clear();
            EndConveyor();
        }
    }
    else
    {
        conv->GetServerModels();
//        emit EndTransactions();
    }
}
//-------------------------------------------------------------------------
void tModelsConveyor::StartReceiveDeleteFiles()
{
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
                stop=conv->ReceiveFile(S);
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
                stop=conv->DeletingFile(S, true);
            }
        }
        conv->AddStartTransaction(false);
        stop=conv->AddReceiveCommand();
        conv->AddDelCommand();

//        conv->AddCommitTransaction(false);
        conv->AddCommitTransactionDel();

        //Начало выполнения списка команд
        if(!stop)
        {
            conv->StartExecution();
        }
        else
        {
            QMessageBox MB;
            MB.setText(QString::fromUtf8("Локальный файл изменился. Операция прервана."));
            MB.setWindowTitle(QString::fromUtf8("Ошибка"));
            MB.exec();

            conv->Clear();
            EndConveyor();
        }
    }
    else
    {
        emit EndTransactions();
    }
}
//-------------------------------------------------------------------------
void tModelsConveyor::CorrectLastSynch()
{
    conv->CorrectLastSynch();

}
//-------------------------------------------------------------------------
void tModelsConveyor::SetTransactionFlag(bool _flag)
{
    Transaction=_flag;
}
