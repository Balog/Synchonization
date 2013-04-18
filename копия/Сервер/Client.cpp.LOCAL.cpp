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
    RD=false;
    handle=_handle;
    Name=QString::number(handle);
    qDebug() << "new socket (Client.cpp)";







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
    socket=new QTcpSocket();
    bool B= socket->setSocketDescriptor(handle);
    qDebug() << "socket thread 1" << socket->thread();
    socket->moveToThread(this->thread());

    qDebug() << "socket thread 2" << socket->thread();
//    socket->setParent(parent);
    connect(socket, SIGNAL(disconnected()),this, SLOT(OnDisconnect()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

//    QAbstractSocket::SocketState St=socket->state();

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
    static int NumCom;
    static QString FileName;
    static qint64 FileSize;
    if(RD)
    {
        if(NumCom==2)
        {
            ReadData(FileSize);
        }
    }
    else
    {
        WaitPackage();



        QString Begin="";



        in >> Begin;
        if(Begin=="Command:")
        {
            NumCom=-1;
            in >> NumCom;

            switch(NumCom)
            {
            case 1:
            {
                FileName="";
                in >> FileName;
                inputFile=new QFile(RootPath+FileName);
                qDebug() << FileName << inputFile->size();
                if(!inputFile)
                {
                    SendErrorReport(NumCom, FileName);
                    return;
                }
                else
                {

                    SendFile(FileName);
                }
                break;
            }
            case 2:
            {
                FileName="";
                FileSize=-1;

                in >> FileName;
                in >> FileSize;

                inputFile=new QFile(RootPath+FileName);
                inputFile->open(QIODevice::WriteOnly);
                RD=true;
                ReadData(FileSize);
                break;
            }
            }




        }
        else
        {
            SendErrorReport(-1, tr("No command"));
            return;
        }

    }

}
//---------------------------------------------------------------------------------------
void Client::OnDisconnect()
{
    emit DisconnectClient(handle);
}
//---------------------------------------------------------------------------------------
void Client::SendFile(QString &FileName)
{
    qDebug() << FileName;
//    QFile inputFile(FILENAME);
    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString Comm="Report:";
    int NumComm=1;

    qint64 FileSize=inputFile->size();
    qDebug() << FileSize;
    out << quint16(0);

    out << Comm;
    out << NumComm;
    out << FileName;
    out << FileSize;

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    socket->write(block);
    socket->waitForBytesWritten();


    inputFile->open(QIODevice::ReadOnly);
        char* buff=new char[32768*8];
    while(1)
    {

        qint64 N=inputFile->read(buff, 32768*8);
        if(N==0)
        {
//            delete[] buff;
            break;
        }
        socket->write(buff, N);
        socket->waitForBytesWritten();



    }
            delete[] buff;
    inputFile->close();
    delete inputFile;
    inputFile=NULL;
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
//---------------------------------------------------------------------------------------
void Client::ReadData(qint64 FS)
{
    qint64 N=socket->bytesAvailable();
    char* buff= new char[N];
    qint64 Num=socket->read(buff, N);
    qint64 WrNum=inputFile->write(buff, Num);

    delete[] buff;

    qint64 FS1=inputFile->size();
    if(inputFile->size()==FS)
    {
        CompletedTransfer();
    }
}
//---------------------------------------------------------------------------------------
void Client::CompletedTransfer()
{



    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString Comm="Report:";
    int NumComm=2;
    QString FN=inputFile->fileName();
    QString RP=RootPath;

    out << quint16(0);

    out << Comm;
    out << NumComm;
    out << FN.right(FN.length()-RP.length());
    out << inputFile->size();

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    socket->write(block);
    socket->waitForBytesWritten();

    inputFile->close();
    delete inputFile;
    inputFile=NULL;
    RD=false;
}
//---------------------------------------------------------------------------------------
