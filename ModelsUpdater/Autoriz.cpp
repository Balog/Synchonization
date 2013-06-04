#include "Autoriz.h"
#include "ui_Autoriz.h"


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
void Autoriz::OnStartAutorizForm()
{
    this->setVisible(true);
    this->setModal(true);
}
//-----------------------------------------------------------------
void Autoriz::on_pbOk_clicked()
{
    this->setModal(false);
    this->setVisible(false);
    login=ui->leLogin->text();
    password=ui->lePassword->text();



    log.Write(QString(QString("Отправка авторизации Логин: ")+login));

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
