#include "tNewPath.h"
#include "ui_tNewPath.h"

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


