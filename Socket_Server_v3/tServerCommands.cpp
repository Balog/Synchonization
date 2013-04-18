#include "tServerCommands.h"

//****************Первая команда***Отправка файла*****************
//----------------------------------------------------------------
tSendFile::tSendFile()
{

}
//----------------------------------------------------------------
tSendFile::~tSendFile()
{

}
//----------------------------------------------------------------
void tSendFile::ExeCommand()
{
    QString FileName;

    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    FileName="";
    in >> FileName;
    inputFile=new QFile(RootPath+FileName);
    qDebug() << FileName << inputFile->size();
    if(!inputFile)
    {
        SendErrorReport(FileName);
        return;
    }
    else
    {
        SendFile(FileName);
    }
}
//----------------------------------------------------------------
void tSendFile::SendFile(QString &_FileName)
{
    qDebug() << _FileName;
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
    out << _FileName;
    out <<FileSize;

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    //    qDebug() << "Write socket thread " << socket->thread();
    socket->write(block);
    socket->waitForBytesWritten();

    inputFile->open(QIODevice::ReadOnly);
    char* buff=new char[32768*8];
    while(1)
    {

        qint64 N=inputFile->read(buff, 32768*8);
        if(N==0)
        {
            break;
        }
        socket->write(buff, N);
        socket->waitForBytesWritten();
    }
    delete[] buff;
    inputFile->close();
    delete inputFile;
    inputFile=NULL;


    emit this->EndCommand();
    //    qDebug() << "Thread : File transfer completed";
}
//----------------------------------------------------------------


//****************Вторая команда****Прием файла*******************
//----------------------------------------------------------------
tReceiveFile::tReceiveFile()
{

}
//----------------------------------------------------------------
tReceiveFile::~tReceiveFile()
{

}
//----------------------------------------------------------------
void tReceiveFile::ExeCommand()
{
    QString FileName;
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadOnly);

    in.setDevice(socket);
    in.setVersion(QDataStream::Qt_4_8);

    FileName="";
    FileSize=-1;

    in >> FileName;
    in >> FileSize;

    inputFile=new QFile(RootPath+FileName);
    inputFile->open(QIODevice::WriteOnly);


    ContinueRead();
}
//----------------------------------------------------------------
void tReceiveFile::ContinueRead()
{
    //    qDebug() << "Продолжение чтения сокета ";

    qint64 N=socket->bytesAvailable();
    char* buff= new char[N];
    qint64 Num=socket->read(buff, N);
    inputFile->write(buff, Num);

    delete[] buff;
    qint64 F1=inputFile->size();
    qint64 F2=FileSize;
    if(inputFile->size()==FileSize)
    {
        FinishReceiveFile();
    }
}
//----------------------------------------------------------------
void tReceiveFile::FinishReceiveFile()
{

    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString Comm="Report:";
    int NumComm=2;

    QString FN=inputFile->fileName();
    QString RP=RootPath;
    QString FN1=FN.right(FN.length()-RP.length());
    qint64 FileSize=inputFile->size();
    qDebug() << FileSize;
    out << quint16(0);

    out << Comm;
    out << NumComm;
    out << FN1;
    out << FileSize;

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    socket->write(block);
    socket->waitForBytesWritten();

    inputFile->close();
    delete inputFile;
    inputFile=NULL;

    emit EndCommand();
}
//----------------------------------------------------------------

