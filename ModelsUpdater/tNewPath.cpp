#include "tNewPath.h"
#include "ui_tNewPath.h"
#include<QDebug>

//extern tSettings my_settings;

tNewPath::tNewPath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tNewPath)
{
    ui->setupUi(this);
}
//-------------------------------------------------------
tNewPath::~tNewPath()
{
    delete ui;
}
//-------------------------------------------------------
void tNewPath::Visible(QString& _login, QString& _message)
{
    qDebug() << "tNewPath::Visible" << _login << _message;
    login=_login;
    ui->teMessage->setText(_message);
    this->setVisible(true);
}

