#include "tProgress.h"
#include "ui_tProgress.h"

tProgress::tProgress(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::FramelessWindowHint),
    ui(new Ui::tProgress)
{
    ui->setupUi(this);

}
//-----------------------------------------------------
tProgress::~tProgress()
{
    delete ui;
}
//-----------------------------------------------------
