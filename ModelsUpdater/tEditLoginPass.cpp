#include "tEditLoginPass.h"
#include "ui_tEditLoginPass.h"

tEditLoginPass::tEditLoginPass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tEditLoginPass)
{
    ui->setupUi(this);

}
//-------------------------------------------------------------------------
tEditLoginPass::~tEditLoginPass()
{
    delete ui;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//void tEditLoginPass::OnSave()
//{

//}
////-------------------------------------------------------------------------
//void tEditLoginPass::OnCancel()
//{
//}
////-------------------------------------------------------------------------
void tEditLoginPass::SetLogin(const QString &_login)
{
    ui->leLogin->setText(_login);

}
//-------------------------------------------------------------------------
QString tEditLoginPass::GetLogin() const
{
    return ui->leLogin->text();
}
//-------------------------------------------------------------------------
QString tEditLoginPass::GetPassword() const
{
    return ui->lePassword->text();
}
//-------------------------------------------------------------------------
void tEditLoginPass::ClearAll()
{
    ui->leLogin->setText("");
    ui->lePassword->setText("");
}
//-------------------------------------------------------------------------
void tEditLoginPass::OnShowEditLogin(bool Visible, bool Modal)
{
    if(!Visible && !Modal)
    {
        ClearAll();
    }
    this->setVisible(Visible);
    this->setModal(Modal);
}
//-------------------------------------------------------------------------
void tEditLoginPass::on_pushButton_clicked()
{
    emit EndEditing(ui->leLogin->text(), ui->lePassword->text(), row, new_user);
}
//-------------------------------------------------------------------------
void tEditLoginPass::on_pushButton_2_clicked()
{

        this->setModal(false);
        this->setVisible(false);
}
