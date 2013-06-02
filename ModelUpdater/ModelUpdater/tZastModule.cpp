#include "tZastModule.h"

tZastModule::tZastModule(QObject *parent) :
    QObject(parent), connect_ok(false), timer2(new QTimer())
{
    log.Write(QString("Zast \tFindServer\tНачало отсчета времени на поиск сервера "));

//    connect_ok=false;

    connect(timer2, SIGNAL(timeout()), this, SLOT(OnTimerFalse()));
    timer2->start(10000);
}
//---------------------------------------------------------------------------------------
tZastModule::~tZastModule()
{
    delete timer2;
}
//---------------------------------------------------------------------------------------
void tZastModule::OnTimerFalse()
{
    delete timer2;
    timer2=NULL;
    emit FindServerFalse();
}
//---------------------------------------------------------------------------------------
bool tZastModule::GetConnect()
{
    return connect_ok;
}
