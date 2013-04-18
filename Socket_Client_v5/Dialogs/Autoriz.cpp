#include "Autoriz.h"
#include "ui_Autoriz.h"

Autoriz::Autoriz(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::Autoriz)
{
    ui->setupUi(this);
}
//-----------------------------------------------------------------
Autoriz::~Autoriz()
{
    delete ui;
}
//-----------------------------------------------------------------
void Autoriz::OnOk()
{
    this->setModal(false);
    this->setVisible(false);
    login=ui->leLogin->text();
    password=ui->lePassword->text();

    emit SendAutorization(login, password);
}
//-----------------------------------------------------------------
void Autoriz::GetAutorization(QString& _login, QString& _password)
{
    _login=login;
    _password=password;
}
