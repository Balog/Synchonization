#include "Client.h"
#include "tFileBlocker.h"

extern tFileBlocker blocker;

tClient::tClient(const int _handle, const QSqlDatabase &_db): QObject(NULL), login(""), socket(NULL), next_block_size(0), Transaction(false)
{
    QString s="Client "+QString::number(_handle);
    tLog log1(s);
    log=log1;

    comm=NULL;

    handle=_handle;

    vf.reg("Command:1",Create_tSendStreamFile);
    vf.reg("Command:2",Create_tReceiveStreamFile);
    vf.reg("Command:3",Create_tConnectionReport);
    vf.reg("Command:4",Create_tPrepareReceiveFile);
    vf.reg("Command:5",Create_tAutorization);
    vf.reg("Command:6",Create_tStartTransaction);
    vf.reg("Command:7",Create_tCommitTransaction);
    vf.reg("Command:8",Create_tCancelTransactionn);
    vf.reg("Command:9",Create_tDeleteFile);
    vf.reg("Command:10",Create_tGetFileList);
    vf.reg("Command:11",Create_tGetListModels);
    vf.reg("Command:12",Create_tSaveLoginPassword);

    db=_db;

    log.Write(QString::fromUtf8("tClient \t tClient \t Клиент создан "));
}
//----------------------------------------------------------------------------
tClient::~tClient()
{
    blocker.UnlockReadWrite(login);

    delete comm;
    comm=NULL;

    socket->deleteLater();

//    db.close();
}
//----------------------------------------------------------------------------
bool tClient::Initialize()
{
    socket=new QTcpSocket();
    bool b= socket->setSocketDescriptor(handle);

    socket->moveToThread(this->thread());
    connect(socket, SIGNAL(disconnected()),this, SLOT(OnDisconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(OnConnected()));

    log.Write(QString::fromUtf8(QString("("+login+") tClient \t Initialize \t Клиент подключен ").toAscii()));

    return b;
}
//---------------------------------------------------------------------------------------
void tClient::OnReadyRead()
{
    int num_com=-1;
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    QByteArray block1;
    QDataStream out(&block1, QIODevice::WriteOnly);

    if(comm)
    {
        // для вариантов постепенного чтения из сокета
        //если нужна последующая запись в сокет репорта об
        //окончании работы по окончании работы выдать false
        //при остальных сеансах - true
        if(!comm->ExeCommand(in, out))
        {


            if(comm) comm->SendReport(out);

            socket->write(block1);
            socket->waitForBytesWritten();
        }
    }
    else
    {
        if(WaitPackage())
        {
            QString begin="";
            in >> begin;
            if(begin=="Command:")
            {
                in >> num_com;

                QString l=QString::fromUtf8(QString("("+login+") tClient \t OnReadyRead \t Получена команда номер "+QString::number(num_com)).toAscii());
                log.Write(l);

                delete comm;
                comm=NULL;
                comm=vf.create(begin+QString::number(num_com));

                connect(comm, SIGNAL(EndCommand()), this, SLOT(OnEndCommand()));

                comm->SetLink(this);
                if(comm->Initialize(in))
                {
                    //для варианта записи в сокет
                    //если запись не нужна а предусмотрено только однократное чтение
                    //вернуть true
                    //
                    //если после однократного чтения нужно что-то отослать в сокет (например Report)
                    //и для этого уже достаточно информации и вся работа сделана командой
                    //то нужно выдать данные в поток и вернуть false и сигнал EndCommand
                    //
                    //если после чтения предусматривается какая-то особая работа
                    //прочитанные данные вернуть особым сигналом который привязывается к слоту
                    //внутри команды а также сигнал EndCommand для завершения текущей команды
                    //в слоте можно организовать создание и запуск другой команды, которая
                    //продолжит обработку принятой информации и организует отправку клиенту
                    //результата если это необходимо
                    while(comm && !comm->ExeCommand(in, out))
                    {
                        qDebug() << block1.size();
                        socket->write(block1);
                        socket->waitForBytesWritten();
                        block1.clear();
                        if(comm) comm->ExternalExit();
                    }
                }
                else
                {
                    QString l=QString::fromUtf8(QString("("+login+") tClient \t OnReadyRead \t Ошибка при выполнении команды номер "+QString::number(num_com)).toAscii());
                    log.Write(l);

                    QByteArray block2;
                    QDataStream in_out(&block2, QIODevice::ReadWrite);

                    comm->SendErrorReport(in_out);

                    socket->write(block2);
                    socket->waitForBytesWritten();

                    delete comm;
                    comm=NULL;
                }
            }
            else
            {
                //Неправильный комманд
                //Мусор в сокете

                log.Write(QString::fromUtf8(QString("("+login+") tClient \t OnReadyRead \t Мусор в сокете. Команда не начинается на 'Command:' ").toAscii()));

                qDebug() << QString::fromUtf8("Мусор в сокете");

                QByteArray block3;
                QDataStream in_out1(&block3, QIODevice::ReadWrite);

                delete comm;
                comm=NULL;
                comm=vf.create("Error");

                connect(comm, SIGNAL(EndCommand()), this, SLOT(OnEndCommand()));

                QString Error="Мусор в сокете. Команда не начинается на 'Command:'";
                in_out1 << Error;

                comm->SendErrorReport(in_out1);

                socket->write(block3);
                socket->waitForBytesWritten();

                delete comm;
                comm=NULL;
            }
        }
    }

}
//---------------------------------------------------------------------------------------
void tClient::OnDisconnect()
{
    QString l=QString::fromUtf8(QString("("+login+") tClient \t OnDisconnect \t Клиент "+QString::number(handle)+" Логин "+login+" отключен ").toAscii());
    log.Write(l);

    emit DisconnectClient(handle);
}
//---------------------------------------------------------------------------------------
bool tClient::WaitPackage()
{
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    if(!next_block_size)
    {
        quint16 b=socket->bytesAvailable();
        quint16 s=sizeof(quint16);
        if(b<s)
        {

            return false;
        }
        in >> next_block_size;

    }
    quint16 avail=socket->bytesAvailable();
    if(avail<next_block_size)
    {

        return false;
    }
    next_block_size=0;
    return true;
}
//---------------------------------------------------------------------------------------
void tClient::OnEndCommand()
{
    delete comm;
    comm=NULL;
}
//---------------------------------------------------------------------------------------
void tClient::OnConnected()
{
    log.Write(QString::fromUtf8(QString("("+login+") tClient \t OnConnected \t Формирование отчета о подключении ").toAscii()));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    delete comm;
    comm=NULL;
    comm=vf.create("Command:3");
    comm->ExeCommand(out, out);

    socket->write(block);
    socket->waitForBytesWritten();

    delete comm;
    comm=NULL;

}
//---------------------------------------------------------------------------------------
void tClient::SetName(const QString& _login)
{
    login=_login;
    qDebug() << "Client name is " << login;

    QString l=QString::fromUtf8(QString("("+login+") tClient \t SetName \t Присвоение логина "+login+" клиенту "+QString::number(handle)).toAscii());
    log.Write(l);

}
//---------------------------------------------------------------------------------------
void tClient::AddDelList(QString &_file_name, QString &_hash)
{
    tFileList fl;
    fl.file_name=_file_name;
    fl.server_hash=_hash;

    del_list.push_back(fl);
}
//---------------------------------------------------------------------------------------
QString tClient::GetDelFile(const int _num) const
{
    return del_list[_num].file_name;
}
//---------------------------------------------------------------------------------------
void tClient::ClearDelList()
{
    del_list.clear();
}
//---------------------------------------------------------------------------------------
QSqlDatabase tClient::GetDB() const
{
    return db;
}
//---------------------------------------------------------------------------------------
QString tClient::GetDelHash(const int _num) const
{
    return del_list[_num].server_hash;
}
//---------------------------------------------------------------------------------------
bool tClient::IsTransaction()
{
    return Transaction;
}
//---------------------------------------------------------------------------------------
void tClient::SetTransaction(bool _trans)
{
    Transaction=_trans;
}
