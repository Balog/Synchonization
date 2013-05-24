#include "dialog.h"
#include "ui_dialog.h"
#include "tSettings.h"

extern tSettings my_settings;
//---------------------------------------------------------------------
Dialog::Dialog(QWidget *_parent) :
    QDialog(_parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint), ui(new Ui::Dialog), server(NULL), base_op(NULL)
{
    tLog log1("GUI");
    log=log1;
    log.Write("Dialog \t Dialog \t Начало работы ");



    QTextCodec *codec =QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

    ui->setupUi(this);

    ui->leRoot->setText(my_settings.GetRoot());
    ui->leTemp->setText(my_settings.GetTemp());
    ui->leAddres->setText(my_settings.GetServerAddr());
    ui->sbPort->setValue(my_settings.GetServerPort());

    OnConnect();

//    base_op=new tDatabaseOp();


}
//---------------------------------------------------------------------
Dialog::~Dialog()
{
//    OnDisconnect();
    delete server;
    server= NULL;

    delete base_op;
    base_op=NULL;

    delete ui;

    log.Write("Dialog \t Dialog \t Конец работы ");
}
//---------------------------------------------------------------------
void Dialog::OnConnect()
{


    my_settings.SetServerAddr(ui->leAddres->text());
    my_settings.SetServerPort(ui->sbPort->value());
    my_settings.SetRoot(ui->leRoot->text());
    my_settings.SetTemp(ui->leTemp->text());
    my_settings.sync();

    server=new tServer(ui->leAddres->text(),ui->sbPort->value(),0);
    ui->pbConnect->setEnabled(false);
    ui->pbDisconnect->setEnabled(true);
    ui->leAddres->setEnabled(false);
    ui->sbPort->setEnabled(false);
    ui->leRoot->setEnabled(false);
    ui->leTemp->setEnabled(false);

    log.Write("Dialog \t Dialog \t Сервер подключен ");

    connect(server, SIGNAL(ConnectClient(int,int)), this, SLOT(OnConnectClient(int,int)));
    connect(server, SIGNAL(DisconnectClient(int ,int)), this, SLOT(OnConnectClient(int ,int)));
}
//---------------------------------------------------------------------
void Dialog::OnDisconnect()
{
    server->DisconnectAll();
    server->close();
    delete server;
    server=NULL;
    ui->leCountClients->setText("0");
    ui->pbConnect->setEnabled(true);
    ui->pbDisconnect->setEnabled(false);

    ui->leAddres->setEnabled(true);
    ui->sbPort->setEnabled(true);
    ui->leRoot->setEnabled(true);
    ui->leTemp->setEnabled(true);

    log.Write("Dialog \t Dialog \t Сервер отключен вручную");
}
//---------------------------------------------------------------------
void Dialog::OnConnectClient(int , int _count_client)
{
    ui->leCountClients->setText(QString::number(_count_client));
}
//---------------------------------------------------------------------
void Dialog::OnAddNew()
{
    server->RefreshDatabase();
//    base_op->AddNewModels();
}
//---------------------------------------------------------------------
void Dialog::OnClearModels()
{
//    base_op->ClearModels();
}
//---------------------------------------------------------------------
