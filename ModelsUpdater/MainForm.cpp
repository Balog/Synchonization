#include "MainForm.h"
#include "ui_MainForm.h"

MainForm::MainForm(QWidget *parent) :
    ui(new Ui::MainForm), QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint),
    main(new tExportMain), zast( new Zast), autoriz(new Autoriz)
{

    ui->setupUi(this);
    this->setVisible(false);
    connect(this, SIGNAL(ZastVisible(bool)), zast, SLOT(FormVisible(bool)));
    connect(main, SIGNAL(FindServer(bool)), this, SLOT(OnFindServer(bool)));
    connect(this, SIGNAL(FindServer(bool)), zast, SLOT(OnFindServer(bool)));
    connect(main, SIGNAL(StartAutorizForm()), autoriz, SLOT(OnStartAutorizForm()));

    emit ZastVisible(true);
}
//--------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete zast;
    delete main;
    delete ui;

}
//---------------------------------------------------------------------
void MainForm::OnFindServer(bool ok)
{

    emit FindServer(ok);
}
//---------------------------------------------------------------------

