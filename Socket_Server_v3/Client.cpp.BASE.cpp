#include "Client.h"
//#define FILENAME "C:/Users/Public/Pictures/Sample Pictures/DXSDK_Jun10.exe"
#define RootPath "C:/Users/Public/Pictures/Sample Pictures/"

//Client::Client(QObject *parent) :
//    QObject(parent)

//{
//    handle=_handle;
//    Name=QString::number(handle);
//    qDebug() << "new socket (Client.cpp)";
//    socket=new QTcpSocket();


//    connect(socket, SIGNAL(disconnected()),this, SLOT(OnDisconnect()));
//    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));


//}
Client::Client(int _handle, QObject *parent): QObject(NULL), Name(""), socket(NULL) //, CS(NULL)
{
//    Comm=-1;
//    FileProcessing=false;
//    _blockSize=0;
//    Preset=new PresetState();
//    Preset->C2.Root="D:/Proba/";

    handle=_handle;
    Name=QString::number(handle);
    qDebug() << "new socket (Client.cpp)";

    socket=new QTcpSocket();
    socket->setParent(parent);
    connect(socket, SIGNAL(disconnected()),this, SLOT(OnDisconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));





}
//----------------------------------------------------------------------------
Client::~Client()
{
//    socket->setParent(NULL);
//    socket->abort();
//    socket->close();
//    qDebug() << "delete socket (Client.cpp)";
//    delete socket;
//    socket=NULL;
    socket->deleteLater();
}
//----------------------------------------------------------------------------
bool Client::Initialize()
{


//    QAbstractSocket::SocketState St=socket->state();
    bool B= socket->setSocketDescriptor(handle);
//    St=socket->state();

    //Это мы запустим в ответ на команду передачи файла а не при подключении
    //SendFile();

    return B;

}
//---------------------------------------------------------------------------------------
void Client::onReadyRead()
{
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    WaitPackage();



    QString Begin="";
    int NumCom=-1;
    FileName="";

    in >> Begin;
    if(Begin=="Command:")
    {
        in >> NumCom;
        in >> FileName;

    }

    inputFile=new QFile(RootPath+FileName);
    if(!inputFile)
    {
        SendErrorReport(NumCom, FileName);
        return;
    }
    else
    {
        SendFile();
    }


//    int Q=socket->bytesAvailable();
//    if(Q!=0)
//    {
//StartSelector(socket, Preset);
//    }

//    QTcpSocket client;
//    qDebug() << "Thread Descriptor :" << handle;
//    if (!client.setSocketDescriptor(socketDescriptor))
//    {
//        qDebug() << client.error();
//        return;
//    }
//    qDebug() << "Thread : Connected";

    //send File

}
//---------------------------------------------------------------------------------------
void Client::OnDisconnect()
{
    emit DisconnectClient(handle);
}
//---------------------------------------------------------------------------------------
void Client::SendFile()
{
//    QFile inputFile(FILENAME);
    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString Comm="Report:";
    int NumComm=1;

    qint64 FileSize=inputFile->size();

    out << quint16(0);

    out << Comm;
    out << NumComm;
    out << FileName;
    out <<FileSize;

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    socket->write(block);
    socket->waitForBytesWritten();


    inputFile->open(QIODevice::ReadOnly);
    while(1)
    {
        char* buff=new char[32768*8];
        qint64 N=inputFile->read(buff, 32768*8);
        if(N==0)
        {
            delete[] buff;
            break;
        }
        socket->write(buff, N);
        socket->waitForBytesWritten();
        delete[] buff;


    }
    inputFile->close();
    delete inputFile;
//    socket->disconnectFromHost();
//    socket->waitForDisconnected();
    qDebug() << "Thread : File transfer completed";
}
//---------------------------------------------------------------------------------------
QString Client::GetName()
{
    return Name;
}
//---------------------------------------------------------------------------------------
int Client::GetHandle()
{
    return handle;
}
//---------------------------------------------------------------------------------------
qint16 Client::WaitPackage()
{
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    quint16 m_nNextBlockSize=0;
    for(;;)
    {

        if(!m_nNextBlockSize)
        {
            for(;;)
            {
                quint16 B=socket->bytesAvailable();
                quint16 S=sizeof(quint16);
                if(B>=S)
                {
                    break;
                }
            }
            in >> m_nNextBlockSize;

        }
        quint16 Avail=socket->bytesAvailable();
        if(Avail>=m_nNextBlockSize)
        {

            break;
        }
    }
    qDebug() << "Package size: " << QString::number(m_nNextBlockSize);
    return m_nNextBlockSize;
}
//---------------------------------------------------------------------------------------
void Client::SendErrorReport(int _NumCom, QString &Mess)
{
    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString Comm="Error:";
    int NumComm=_NumCom;
    QString FileName=Mess;

    out << quint16(0);

    out << Comm;
    out << NumComm;
    out << FileName;

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    socket->write(block);
    socket->waitForBytesWritten();
}
