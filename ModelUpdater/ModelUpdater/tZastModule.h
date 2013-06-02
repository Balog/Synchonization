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
    bool GetConnect();
private:
    bool connect_ok;
//    QTimer *timer1;
    QTimer *timer2;
    tLog log;
    
signals:
    void FindServerFalse(); //Сигнал что отведенное на поиск сервера время истекло
    
public slots:
    void OnTimerFalse();
    
};

#endif // TZASTMODULE_H
