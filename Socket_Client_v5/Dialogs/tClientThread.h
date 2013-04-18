#ifndef TCLIENTTHREAD_H
#define TCLIENTTHREAD_H

#include <QObject>
#include <QThread>
#include "socket.h"

class tClientThread : public QThread
{
    Q_OBJECT
public:
    explicit tClientThread(const QString _addres, const int _port);
    ~tClientThread();

private:
    tSocket *my_socket;
    QString address;
    int port;
    QString root;
    virtual void run();
    
signals:
    void Disconnect();
    void Err(int num_error, QString error, QString detail, QString client_detail);

    void SendCommandSocket(QByteArray block);
    void SendCommandGui(QByteArray block);
    void DisconnectFromServer();
    
private slots:
    void OnDisconnect();
    void OnCommandToSocket(QByteArray block);
    void OnCommandToGui(QByteArray block);
    void OnDisconnecting();
};

#endif // TCLIENTTHREAD_H
