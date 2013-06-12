#include "tZastModule.h"
#include <QMessageBox>
#include<QDebug>

tZastModule::tZastModule(QObject *parent) :
    QObject(parent)//, connect_ok(false), timer2(NULL)
{
//    log.Write(QString("Zast \tFindServer\tНачало отсчета времени на поиск сервера "));

////    connect_ok=false;

}
//---------------------------------------------------------------------------------------
tZastModule::~tZastModule()
{
//    qDebug() << "Удаление timer";
//    delete timer2;
//    timer2=NULL;
}
//---------------------------------------------------------------------------------------
//void tZastModule::StartFindServer()
//{
//    timer2=new QTimer();
//    qDebug() << timer2 << "Запуск таймера";
//    connect(timer2, SIGNAL(timeout()), this, SLOT(OnTimerFalse()));
//    timer2->start(10000);
//}
////---------------------------------------------------------------------------------------
//void tZastModule::OnTimerFalse()
//{
//    qDebug() << timer2;
//    qDebug() << "Время на поиск сервера истекло";
//    log.Write(QString("tZastModule \t OnTimerFalse \t Время на поиск сервера истекло"));

//    delete timer2;
//    timer2=NULL;


//    qDebug() << timer2;
//    qDebug() << "Испущен сигнал ServerFinding(false)";
//    emit FindServer(false);
//}
////---------------------------------------------------------------------------------------
//bool tZastModule::GetConnect()
//{
//    return connect_ok;
//}
////---------------------------------------------------------------------------------------
//void tZastModule::OnTimerTrue()
//{
//qDebug() << "Сервер найден вовремя";

//delete timer2;
//timer2=NULL;
//    log.Write(QString("tZastModule \t OnTimerTrue \t Сервер найден"));

//    qDebug() << "Испущен сигнал ServerFinding(true)";
//    emit FindServer(true);
//}
////---------------------------------------------------------------------------------------
//void tZastModule::OnReconnect()
//{
//    emit Reconnect();
//}
////---------------------------------------------------------------------------------------

