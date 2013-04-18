#include "tThreadClient.h"

//---------------------------------------------------------------------------------------
tThreadClient::tThreadClient(const int _handle, QSqlDatabase &_db):my_thread(NULL),this_client(NULL)
{
    my_thread= new QThread();
    this->moveToThread(my_thread);
    this_client=new tClient(_handle, _db);
    this_client->moveToThread(my_thread);
    connect(this_client, SIGNAL(DisconnectClient(int)), this, SLOT(OnDisconnectClient(int)));
    connect(this, SIGNAL(ConnectClient()), this_client, SLOT(OnConnected()));

}
//---------------------------------------------------------------------------------------
tThreadClient::~tThreadClient()
{
    delete this_client;
    this_client=NULL;

    my_thread->quit();
    my_thread->deleteLater();


    for(;!my_thread->isFinished();){}
    delete my_thread;
}
//---------------------------------------------------------------------------------------
bool tThreadClient::Start()
{
    my_thread->start();
    return this_client->Initialize();
}
//---------------------------------------------------------------------------------------
void tThreadClient::OnDisconnectClient(const int _handle)
{
    emit DisconnectClient(_handle);
}
//---------------------------------------------------------------------------------------
int tThreadClient::GetHandle()
{
    return this_client->GetHandle();
}
//---------------------------------------------------------------------------------------
QString tThreadClient::GetName() const
{
    return this_client->GetName();
}
//---------------------------------------------------------------------------------------
void tThreadClient::Connect()
{
    emit ConnectClient();
}
