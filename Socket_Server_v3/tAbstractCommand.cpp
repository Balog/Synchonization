#include "tAbstractCommand.h"


void tAbstractCommand::SetRootPath(QString& _Root)
{
    RootPath=_Root;
}
//--------------------------------------------------------------
void tAbstractCommand::SetSocket(QTcpSocket* _socket)
{
    socket=_socket;
}

//--------------------------------------------------------------
void tAbstractCommand::SendErrorReport(QString &_Mess)
{
    QByteArray block;

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString Comm="Error:";
    QString FileName=_Mess;

    out << quint16(0);

    out << Comm;
    out << -1;
    out << FileName;

    out.device()->seek(0);
    quint16 BS=(quint16)(block.size() - sizeof(quint16));
    out << BS;

    socket->write(block);
    socket->waitForBytesWritten();
}
//----------------------------------------------------------------
