#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QStringList>
#include "ClassFactory.h"
//#include "tDatabaseOp.h"



#include "tAbstractStreamCommand.h"


#include "tServerStreamCommands.h"

class tClient : public QObject
{
    Q_OBJECT
public:
    tClient(const int handle, const QSqlDatabase &_db);
    ~tClient();
    bool Initialize();
    int GetHandle(){return handle;}
    QString GetName(){return login;}
    void SetName(const QString &login);
    void AddDelList(QString& _file_name, QString &_hash);
    QString GetDelFile(const int _num) const;
    QString GetDelHash(const int _num) const;
    void ClearDelList();
    int GetSizeDelFileList(){return del_list.size();}
    QSqlDatabase GetDB() const;

    QTcpSocket* socket;

private:
    bool WaitPackage();

    vfactory<tAbstractStreamCommand, QString>vf;
    tAbstractStreamCommand* comm;
    QString login;

    struct tFileList
    {
        QString file_name;
        QString server_hash;
    };


    QList<tFileList> del_list;
    int handle;
    QSqlDatabase db;
    quint16 next_block_size;

signals:
    void DisconnectClient(int);
    
private slots:
    void OnDisconnect();
    void OnReadyRead();
    void OnEndCommand();
    void OnConnected();
};

#endif // CLIENT_H
