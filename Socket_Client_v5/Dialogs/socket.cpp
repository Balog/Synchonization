#include "socket.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QFile>




//-------------------------------------------------------------------------------
tSocket::tSocket(const QString &_address, const int _port, const QString &_root, QThread *_owner) :
    comm(NULL), socket(NULL), next_block_size(0)
{
//    qDebug() << "Поток Socket в самом начале " << this->thread() ;

    root=_root;
    address=_address;
    port=_port;
    owner=_owner;

    vf.reg("Error:0",Create_tProcessingError);
    vf.reg("Report:1",Create_tReceiveFile);
    vf.reg("Report:2",Create_tReportSendFile);
    vf.reg("PrepareReceiveFile", Create_tPrepareReceiveFile);
    vf.reg("SendFile", Create_tSendFile);
    vf.reg("Report:3",Create_tConnectConfirmReport);
    vf.reg("PrepareSendFile",Create_tStreamPrepareSendFile);
    vf.reg("Report:4",Create_tStreamReportPrepareSendFile);
    vf.reg("SendAutorization",Create_tStreamSendAutorization);
    vf.reg("Report:5",Create_tStreamReportAutorization);
    vf.reg("StartTransaction",Create_tStreamStartTransaction);
    vf.reg("Report:6",Create_tStreamReportStartTransaction);
    vf.reg("CommitTransaction",Create_tStreamCommitTransaction);
    vf.reg("Report:7",Create_tStreamReportCommitTransaction);
    vf.reg("CancelTransaction",Create_tStreamCancelTransaction);
    vf.reg("Report:8",Create_tStreamReportCancelTransaction);
    vf.reg("DeleteFile",Create_tDeleteFile);
    vf.reg("Report:9",Create_tReportDeleteFile);
    vf.reg("GetListFiles",Create_tGetListFiles);
    vf.reg("Report:10",Create_tReportGetListFiles);
    vf.reg("GetListModels",Create_tGetServerListModels);
    vf.reg("Report:11",Create_tReportGetListServerModels);
}
//-------------------------------------------------------------------------------
tSocket::~tSocket()
{
    delete comm;
    comm=NULL;

    l="socket \tInitialize\tОтключение и удаление сокета ";
    log.Write(l);

    socket->disconnectFromHost();
    delete socket;
}
//-------------------------------------------------------------------------------
void tSocket::Initialize()
{
    l="socket \tInitialize\tСоздание сокета ";
    log.Write(l);

    socket=new QTcpSocket();

    QHostAddress addr(address);
    socket->connectToHost(addr, port);

    connect(socket, SIGNAL(readyRead()),this, SLOT(ReadReport()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(OnDisconnected()));
    connect(this, SIGNAL(Result(QByteArray)), owner, SLOT(OnCommandToGui(QByteArray)));
}
//-------------------------------------------------------------------------------
void tSocket::ReadReport()
{

    int num_com=-1;
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    if(comm)
    {
        comm->ExeCommand(in);
    }
    else
    {
        if(WaitPackage())
        {
            QString begin="";
            in >> begin;
            if(begin=="Report:")
            {
                in >> num_com;


                l="socket \tReadReport\tReport: "+QString::number(num_com);
                log.Write(l);

                delete comm;
                comm=NULL;
                comm=vf.create(begin+QString::number(num_com));

                connect(comm, SIGNAL(EndCommand()), this, SLOT(OnEndCommand()));
                connect(comm, SIGNAL(Result(QByteArray)), owner, SLOT(OnCommandToGui(QByteArray)));

                if(comm->Initialize(in))
                {
                    comm->ExeCommand(in);
//                    l="socket \tReadReport\tКоманда выполнена";
//                    log.Write(l);
                }
                else
                {
                    l="socket \tReadReport\tАльтернатива в выполнении команды";
                    log.Write(l);
                    comm->ProcessError(in);
                }
            }
            else
            {
                if(begin=="Error:")
                {

                    //если команда после Error больше нуля то обработка ведется методом соответствующего Command
                    //если номер ноль или меньше то для этого будут использоваться отдельные команды Error
                    in >> num_com;

                    l="socket \tReadReport\tПринято сообщение об ошибке"+QString::number(num_com);
                    log.Write(l);

                    delete comm;
                    comm=NULL;

                    if(num_com>0)
                    {
                        comm=vf.create("Report:"+QString::number(num_com));
                        connect(comm, SIGNAL(EndCommand()), this, SLOT(OnEndCommand()));
                        connect(comm, SIGNAL(Result(QByteArray)), owner, SLOT(OnCommandToGui(QByteArray)));
                        comm->ProcessError(in);

                        delete comm;
                        comm=NULL;

                    }
                    else
                    {
                        comm=vf.create("Error:"+QString::number(num_com));
                        connect(comm, SIGNAL(EndCommand()), this, SLOT(OnEndCommand()));
                        connect(comm, SIGNAL(Result(QByteArray)), owner, SLOT(OnCommandToGui(QByteArray)));
                        comm->ProcessError(in);

                        delete comm;
                        comm=NULL;
                    }
                }
                else
                {
                    //Неправильный репорт
                    //Мусор в сокете
                    qDebug() << "Мусор в сокете";

                    l="socket \tReadReport\tМусор в сокете"+QString::number(num_com);
                    log.Write(l);

                    int num_error=-1;
                    QString error="";
                    QString detail="";
                    QString client_detail="Ошибка на стороне сервера \nМусор в сокете\nОтвет не начинается на 'Report:' или 'Error:'";

                    QByteArray block;
                    QDataStream in(&block, QIODevice::WriteOnly);

                    in << tr("Error");
                    in << num_error;
                    in << error;
                    in << detail;
                    in << client_detail;

                    emit Result(block);
                }
            }
        }
    }

}
//-------------------------------------------------------------------------------
bool tSocket::WaitPackage()
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
void tSocket::OnEndCommand()
{
    delete comm;
    comm=NULL;
}
//---------------------------------------------------------------------------------------
void tSocket::OnDisconnected()
{
    l="socket \tOnDisconnected\tОтключение сервером ";
    log.Write(l);

    emit DisconnectSocket();
}
//---------------------------------------------------------------------------------------
void tSocket::OnCommand(QByteArray _block)
{


    QDataStream in(&_block, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);

    QString command="";


    in >> command;

    l=tr("socket \tOnCommand\tПолучена команда ")+command;
    log.Write(l);

    delete comm;
    comm=NULL;
    comm=vf.create(command);

    connect(comm, SIGNAL(EndCommand()), this, SLOT(OnEndCommand()));
    connect(comm, SIGNAL(Result(QByteArray)), owner, SLOT(OnCommandToGui(QByteArray)));

    if(comm->Initialize(in))
    {

        QByteArray block1;

        QDataStream out(&block1, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        while(comm!=NULL && !comm->ExeCommand(out))
        {
            socket->write(block1);
            socket->waitForBytesWritten();

            block1.clear();

            if(comm) comm->ExternalExit();
        }

    }
    else
    {
        l="socket \tOnCommand\tОшибка выполнения команды "+command;
        log.Write(l);

        QString comm="";
        int num_error=-1;
        QString error="";
        QString detail="";
        QString client_detail="";

        comm=tr("Error");
        in >> num_error;
        in >> error;
        in >> detail;
        in >> client_detail;

        QByteArray block1;

        QDataStream out(&block1, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);

        out << comm;
        out << num_error;
        out << error;
        out << detail;
        out << client_detail;

        emit Result(block1);
    }

    delete comm;
    comm=NULL;
}
//---------------------------------------------------------------------------------------
void tSocket::OnDisconnectedFromServer()
{

    l="socket \tОтключение от сервера ";
    log.Write(l);

    socket->disconnectFromHost();
}
