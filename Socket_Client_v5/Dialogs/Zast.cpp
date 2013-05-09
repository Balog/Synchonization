#include "Zast.h"
#include "ui_Zast.h"

#include <QFileDialog>
#include <QMessageBox>

//---------------------------------------------------------------------
Zast::Zast(QWidget *_parent) :
    QDialog(_parent), ui(new Ui::Zast), connect_ok(false), main_form(new MainForm), dAutoriz(new Autoriz), timer1(NULL), timer2(NULL)
{
    ui->setupUi(this);
    dAutoriz->setModal(true);

    connect(dAutoriz, SIGNAL(SendAutorization(QString&, QString&)), main_form, SLOT(Autorization(QString&, QString&)));
    connect(main_form, SIGNAL(AutorizStart()), this, SLOT(AutorizStart()));
    connect(main_form, SIGNAL(FindServer()), this, SLOT(FindServer()));
    connect(this, SIGNAL(ReceiveLoginsTable()), main_form, SLOT(OnReceiveLoginsTable()));

    FindServer();
}
//---------------------------------------------------------------------
Zast::~Zast()
{
    delete main_form;
    delete dAutoriz;
    delete ui;
}
//---------------------------------------------------------------------
void Zast::OnTimerTrue()
{

    delete timer2;
    timer2=NULL;
    this->setVisible(false);
    if(connect_ok)
    {

        delete timer1;
        timer1=NULL;

        //сюда встроить команду запроса списка логинов
        emit ReceiveLoginsTable();
        //эту команду вставить в конце обработки списка логинов
//        dAutoriz->show();
    }
}
//---------------------------------------------------------------------
void Zast::OnTimerFalse()
{
    delete timer1;
    timer1=NULL;

    delete timer2;
    timer2=NULL;
    this->setVisible(false);
    this->setModal(false);

    l="Zast \tOnTimerFalse\tНе обнаружен сервер ";
    log.Write(l);

    QMessageBox mb;
    mb.setText(QString::fromUtf8("Не обнаружен сервер"));
    mb.setWindowTitle(QString::fromUtf8("Подключение"));
    mb.exec();
    this->close();
}
//---------------------------------------------------------------------
void Zast::mousePressEvent(QMouseEvent* event)
{
    this->setVisible(false);
    if(event->button()==Qt::LeftButton && connect_ok)
    {
        OnTimerTrue();
    }

}
//---------------------------------------------------------------------
void Zast::AutorizStart()
{
    //    this->setVisible(true);
    l="Zast \tAutorizStart\tСервер найден ";
    log.Write(l);

    timer1=new QTimer();
    connect(timer1, SIGNAL(timeout()), this, SLOT(OnTimerTrue()));
    timer1->start(3000);
    connect_ok=true;
}
//---------------------------------------------------------------------
void Zast::FindServer()
{
    l="Zast \tFindServer\tНачало поиска сервера ";
    log.Write(l);

    connect_ok=false;
    this->setVisible(true);
    this->setModal(true);
    timer2=new QTimer();
    connect(timer2, SIGNAL(timeout()), this, SLOT(OnTimerFalse()));
    timer2->start(10000);
}
//---------------------------------------------------------------------
