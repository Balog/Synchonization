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
void tProgress::Start(int max, int _x, int _y, int _w, int _h)
{

    int w=this->geometry().width();
    int h=this->geometry().height();

    int x=_x+(_w-w)/2;
    int y=_y+(_h-h)/2;

    this->setGeometry(x, y, w, h);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(max);
    ui->progressBar->setFormat(QString("%p"));
    ui->progressBar->setTextVisible(true);
    this->setModal(true);
    this->setVisible(true);
}
//-----------------------------------------------------
void tProgress::setValue(int _value)
{
    ui->progressBar->setValue(_value);


}
//-----------------------------------------------------
void tProgress::Stop()
{
    this->setModal(false);
    this->setVisible(false);
}
