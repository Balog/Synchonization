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

