#include "tClientThread.h"

//-----------------------------------------------------------------------------------------------------------
tClientThread::tClientThread(const QString _addres, const int _port) : my_socket(NULL)
{
    address=_addres;
    port=_port;



}
//-----------------------------------------------------------------------------------------------------------
void tClientThread::run()
{
    my_socket=new tSocket(address, port, root, this);

    connect (my_socket, SIGNAL(DisconnectSocket()), this, SLOT(OnDisconnect()));
    connect(this, SIGNAL(SendCommandSocket(QByteArray)), my_socket, SLOT(OnCommand(QByteArray)));
    connect(my_socket, SIGNAL(SendCommand(QByteArray)),this , SLOT(OnCommandToGui(QByteArray)));
    connect(this, SIGNAL(DisconnectFromServer()), my_socket, SLOT(OnDisconnectedFromServer()));

    my_socket->Initialize();
    QThread::run();
}

//-----------------------------------------------------------------------------------------------------------
tClientThread::~tClientThread()
{
    my_socket->deleteLater();

    quit();

    for(;!isFinished();){}
}
//-----------------------------------------------------------------------------------------------------------
void tClientThread::OnDisconnect()
{
    emit Disconnect();
}
//-----------------------------------------------------------------------------------------------------------
void tClientThread::OnCommandToSocket(QByteArray block)
{
    emit SendCommandSocket(block);
}
//-----------------------------------------------------------------------------------------------------------
void tClientThread::OnCommandToGui(QByteArray block)
{
    emit SendCommandGui(block);
}
//-----------------------------------------------------------------------------------------------------------
void tClientThread::OnDisconnecting()
{
    emit DisconnectFromServer();
}
//-----------------------------------------------------------------------------------------------------------
