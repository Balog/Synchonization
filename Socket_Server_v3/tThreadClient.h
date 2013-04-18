#ifndef TTHREADCLIENT_H
#define TTHREADCLIENT_H

#include <QObject>
#include<QTcpSocket>
#include<QThread>
#include "Client.h"

class tThreadClient : public QObject
{
    Q_OBJECT
public:
    tThreadClient(const int _handle, QSqlDatabase& _db);
    ~tThreadClient();
    void Remove();
    bool Start();
    int GetHandle();
    QString GetName() const;
    int Size;
    void Connect();

private:
    QThread* my_thread;
    tClient* this_client;

    
signals:
    void ConnectClient();
    void DisconnectClient(int _handle);

    
public slots:
    void OnDisconnectClient(const int _handle);
};

#endif // TTHREADCLIENT_H
