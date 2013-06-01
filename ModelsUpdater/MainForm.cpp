#include "MainForm.h"
#include "ui_MainForm.h"

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint),
    MModule(NULL)
{
    ui->setupUi(this);

    MModule=new MainModule;
    connect(MModule, SIGNAL(Proba()), this, SLOT(OnProba()));
    MModule->SetProba();
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{

    delete MModule;
    delete ui;

}
//---------------------------------------------------------------------
void MainForm::OnProba()
{
    qDebug() << "Proba";
}
