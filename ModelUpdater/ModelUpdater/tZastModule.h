#ifndef TZASTMODULE_H
#define TZASTMODULE_H

#include <QObject>
#include <QTimer>
#include"tLog.h"

class tZastModule : public QObject
{
    Q_OBJECT
public:
    explicit tZastModule(QObject *parent = 0);
    ~tZastModule();
//    bool GetConnect();
//    void StartFindServer();

private:
//    bool connect_ok;
////    QTimer *timer1;
//    QTimer *timer2;
//    tLog log;
    
signals:
//    void FindServer(bool ok); //Сигнал что отведенное на поиск сервера время истекло
//    void ServerFinding(); //Сигнал что сервер найден
//    void Reconnect();
    
public slots:
//    void OnTimerFalse();
//    void OnTimerTrue();
//    void OnReconnect();
    
};

#endif // TZASTMODULE_H
