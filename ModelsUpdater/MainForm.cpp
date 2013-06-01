#include "MainForm.h"
#include "ui_MainForm.h"

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint)
{


    ui->setupUi(this);

}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{


    delete ui;

}
//---------------------------------------------------------------------
