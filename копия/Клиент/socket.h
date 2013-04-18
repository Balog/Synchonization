#ifndef SOCKET_H
#define SOCKET_H

#include <QTcpSocket>
#include <QFile>
#include "ClassFactory.h"
#include "tAbstractStreamCommand.h"
#include "tClientStreamCommands.h"

class tSocket : public QObject
{
    Q_OBJECT
public:
    explicit tSocket(const QString &addres, const int port, const QString &_root, QThread* _owner);
    ~tSocket();
    void Initialize();

private:
    QTcpSocket* socket;
    qint64 file_size;
    QString address;
    int port;
    QString root;
    QThread* owner;

    tVfactory<tAbstractStreamCommand, QString>vf;
    tAbstractStreamCommand* comm;

    bool WaitPackage();
    quint16 next_block_size;
    
signals:
    void DisconnectSocket();
    void SendCommand(QByteArray block);
    void Result(QByteArray block);
    
private slots:
    void ReadReport();
    void OnEndCommand();
    void OnDisconnected();
    void OnCommand(QByteArray _block);
    void OnDisconnectedFromServer();
};

#endif // SOCKET_H
