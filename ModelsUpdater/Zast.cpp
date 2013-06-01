#include "Zast.h"
#include "ui_Zast.h"


//---------------------------------------------------------------------
Zast::Zast(QWidget *_parent) :
    QDialog(_parent, Qt::WindowSystemMenuHint | Qt::FramelessWindowHint), ui(new Ui::Zast)
{
    ui->setupUi(this);
}
//---------------------------------------------------------------------
Zast::~Zast()
{

    delete ui;
}
//---------------------------------------------------------------------
