#include "Server.h"
#include <QtNetwork>

extern tSettings my_settings;
tFileBlocker blocker;

//------------------------------------------------------------------------------
tServer::tServer(const QString& _adr, const int _port, QObject *_parent) : QTcpServer(_parent)
{
    tLog log1("GUI");
    log=log1;



blocker.SetMaxReaders(my_settings.GetMaxReaders());
    QHostAddress addr(_adr);
    if(listen(addr, _port))
    {
//        qDebug() << QString::fromUtf8("Подключен на адрес "+_adr+" и порт "+QString::number(_port));
    }
    else
    {
        qDebug() <<QString::fromUtf8("Не подключено");
    }


//    tDatabaseOp *db_op=new tDatabaseOp();
//    db_op->RefreshModelsFiles();
//    delete db_op;
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Database.db");
    if(!db.open()){qDebug() << QString::fromUtf8(db.lastError().text().toAscii());}


    RefreshDatabase();
}
//------------------------------------------------------------------------------
tServer::~tServer()
{
    db.close();


    this->close();
    foreach(tThreadClient* i,clients)
    {
        //ТУТ НЕОБХОДИМО ПРОЙТИ ПО МЭПУ И УДАЛИТЬ ВСЕ ЗАПИСИ С МЕТКОЙ УДАЛЯЕМОГО КЛИЕНТА.

        delete i;
        i=NULL;
        break;
    }
    clients.clear();


}
//------------------------------------------------------------------------------
void tServer::incomingConnection(int _socket_descriptor)
{
    QString l=QString::fromUtf8("tServer \t incomingConnection \t Входящее соединение. Дескриптор: ")+QString::number(_socket_descriptor);
    log.Write(l);
    my_thread=new tThreadClient(_socket_descriptor, db);

    if(my_thread->Start())
    {
        clients.push_back(my_thread);
        emit ConnectClient(_socket_descriptor, clients.size());
        my_thread->Connect();
    }
    else
    {
        delete my_thread;
        my_thread=NULL;
    }
    connect(my_thread, SIGNAL(DisconnectClient(int)), this, SLOT(OnDisconnectClient(int)));

}
//------------------------------------------------------------------------------
void tServer::OnDisconnectClient(const int _handle)
{
    vector <tThreadClient*>::const_iterator It=clients.begin();
    foreach(tThreadClient* i,clients)
    {
        int H=i->GetHandle();
        if(_handle==H)
        {
            delete i;
            i=NULL;
            clients.erase(It);
            break;
        }
        It++;
    }
    int Count=clients.size();

    QString l=QString::fromUtf8("tServer \t OnDisconnectClient \t Клиент. Дескриптор: ")+QString::number(_handle)+QString::fromUtf8(" отключен. Осталось ") + QString::number(Count) + QString::fromUtf8(" клиентов");

    log.Write(l);

    emit DisconnectClient(_handle, Count);
}
//------------------------------------------------------------------------------
void tServer::DisconnectAll()
{
    foreach(tThreadClient* i,clients)
    {
        i->disconnect();
        delete i;
    }
    clients.clear();


    log.Write(QString::fromUtf8("tServer \t DisconnectAll \t Отключены все клиенты "));
}
//------------------------------------------------------------------------------
void tServer::RefreshDatabase()
{
    log.Write(QString::fromUtf8("tServer \t RefreshDatabase \t Проверка и обновление базы данных файлов "));
    tDatabaseOp db_op(db);
        db_op.RefreshModelsFiles();
}
