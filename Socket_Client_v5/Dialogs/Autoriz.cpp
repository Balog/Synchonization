#include "Autoriz.h"
#include "ui_Autoriz.h"
#include "tLog.h"

Autoriz::Autoriz(QWidget *_parent) :
    QDialog(_parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
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

    tLog log;
    QString l="Отправка авторизации Логин: "+login;
    log.Write(l);

    bool mod_fold=false;
    if(ui->cbModifyFolder->checkState()==Qt::Checked)
    {
        mod_fold=true;
    }
    ui->leLogin->setText("");
    ui->lePassword->setText("");
    ui->cbModifyFolder->setChecked(false);
    emit SendAutorization(login, password, mod_fold);


}
//-----------------------------------------------------------------
void Autoriz::GetAutorization(QString& _login, QString& _password)
{
    _login=login;
    _password=password;
}
