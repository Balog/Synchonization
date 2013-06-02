#include "Zast.h"
#include "ui_Zast.h"
#include <QMessageBox>



//---------------------------------------------------------------------
Zast::Zast(QWidget *_parent) :
    QDialog(_parent, Qt::WindowSystemMenuHint | Qt::FramelessWindowHint), ui(new Ui::Zast)
{
    ui->setupUi(this);
    QPixmap myPixmap(":/Pictures/Zast.png");
    ui->label->setPixmap(myPixmap);

    this->setVisible(true);
    this->setModal(true);
}
//---------------------------------------------------------------------
Zast::~Zast()
{

    delete ui;
}
//---------------------------------------------------------------------
void Zast::mousePressEvent(QMouseEvent* event)
{
    this->setVisible(false);
    if(event->button()==Qt::LeftButton)
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


void Zast::OnReconnect()
{
    this->setVisible(true);
    this->setModal(true);
}
//---------------------------------------------------------------------
void Zast::FormVisible(bool vis)
{
    this->setVisible(vis);
    this->setModal(vis);
}
