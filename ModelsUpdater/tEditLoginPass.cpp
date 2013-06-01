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
