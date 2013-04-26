#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <vector>
#include "tThreadClient.h"
#include "tFileBlocker.h"
#include <QtSql>
#include "tDatabaseOp.h"
#include "tLog.h"

using namespace std;



class tServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit tServer(const QString &_adr, const int _port, QObject *_parent = 0);
    ~tServer();
    void DisconnectAll();
    void RefreshDatabase();

private:
    tThreadClient* my_thread;
    vector <tThreadClient*> clients;
    QSqlDatabase db;

    tLog log;


protected:
    void incomingConnection(int _socket_descriptor);
    
signals:
    void ConnectClient(int handle, int count_clients);
    void DisconnectClient(int handle, int count_clients);

public slots:
    void OnDisconnectClient(const int handle);

};

#endif // SERVER_H
