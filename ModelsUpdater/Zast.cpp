#include "Zast.h"
#include "ui_Zast.h"
#include <QMessageBox>



//---------------------------------------------------------------------
Zast::Zast(QWidget *_parent) :
    QDialog(_parent, Qt::WindowSystemMenuHint | Qt::FramelessWindowHint), ui(new Ui::Zast),
    zast(new tExportZast), main_form(new MainForm)
{
    ui->setupUi(this);
    QPixmap myPixmap(":/Pictures/Zast.png");
    ui->label->setPixmap(myPixmap);

    this->setVisible(true);
    this->setModal(true);

        connect(zast, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
//        connect(zast, SIGNAL(FindServerTrue()), this, SLOT(OnFindServerTrue()));
//        connect(zast, SIGNAL(Reconnect()), this, SLOT(OnReconnect()));
//    FindServer();
    zast->StartFindServer();
}
//---------------------------------------------------------------------
Zast::~Zast()
{
    delete main_form;
    delete zast;
    delete ui;
}
//---------------------------------------------------------------------
void Zast::mousePressEvent(QMouseEvent* event)
{
    this->setVisible(false);
    if(event->button()==Qt::LeftButton && zast->GetConnect())
    {
        this->setModal(false);
    }

}
//---------------------------------------------------------------------
void Zast::OnFindServer(bool ok)
{
    if(ok)
    {

        this->setVisible(false);
        this->setModal(false);
    }
    else
    {
        this->close();

        QMessageBox mb;
        mb.setText(QString::fromUtf8("Не обнаружен сервер"));
        mb.setWindowTitle(QString::fromUtf8("Подключение"));
        mb.exec();
    }
}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
void Zast::OnReconnect()
{
    this->setVisible(true);
    this->setModal(true);
}

//void Zast::OnTimerTrue()
//{

//    delete timer2;
//    timer2=NULL;
//    this->setVisible(false);
//    if(connect_ok)
//    {

//        delete timer1;
//        timer1=NULL;

//        //сюда встроить команду запроса списка логинов
//        emit ReceiveLoginsTable();
//        //эту команду вставить в конце обработки списка логинов

//    }
//}
////---------------------------------------------------------------------
//void Zast::FindServer()
//{

//    log.Write(QString("Zast \tFindServer\tНачало отсчета времени на поиск сервера "));

////    connect_ok=false;

//    timer2=new QTimer();
//    connect(timer2, SIGNAL(timeout()), this, SLOT(OnTimerFalse()));
//    timer2->start(10000);
//}
////---------------------------------------------------------------------
//void Zast::OnTimerFalse()
//{
//    delete timer1;
//    timer1=NULL;

//    delete timer2;
//    timer2=NULL;
//    this->setVisible(false);
//    this->setModal(false);

//    log.Write(QString("Zast \tOnTimerFalse\tНе обнаружен сервер "));

//    QMessageBox mb;
//    mb.setText(QString::fromUtf8("Не обнаружен сервер"));
//    mb.setWindowTitle(QString::fromUtf8("Подключение"));
//    mb.exec();
//    this->close();
//}
////---------------------------------------------------------------------
