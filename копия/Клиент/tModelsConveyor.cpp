#include "tModelsConveyor.h"
#include <QMessageBox>

tModelsConveyor::tModelsConveyor(Ui::MainForm *_ui, QObject* _link, tDatabaseOp *_db_op, QObject *parent) :
    QObject(parent), ui(_ui), db_op(_db_op), link(_link)
{
    db_op=new tDatabaseOp();

    db_op->RefreshModelsFiles();

    conv=new tConveyor(ui, db_op, link);
//    conv->SetLink(link);

    connect(this, SIGNAL(NextCommand()), conv, SLOT(NextCommand()));
    connect(conv, SIGNAL(EndCommands()), this, SLOT(EndConveyor()));

    connect(this, SIGNAL(RunGui(QByteArray&)), conv, SLOT(OnRunGuiCommand(QByteArray&)));
    connect(conv, SIGNAL(Disconnect()), this, SLOT(OnDisconnect()));
    connect(conv, SIGNAL(CloseMain()), this, SLOT(OnClose()));
    connect(conv, SIGNAL(AutorizStart()), this, SLOT(OnAutorizStart()));
    connect(conv, SIGNAL(SetVisible(bool)),this, SLOT(OnSetVisible(bool)));
    connect(this, SIGNAL(Disconnecting()), conv, SLOT(OnDisconnecting()));
    connect(conv, SIGNAL(ErrorCommands()),this, SLOT(ErrorConveyor()));





//    conv->SetDatabaseOperator(db_op);

}
//-------------------------------------------------------------------------
void tModelsConveyor::StartServer(const QString &_addr, const int _port)
{
conv->StartServer(_addr, _port);
}
//-------------------------------------------------------------------------
void tModelsConveyor::Clear()
{
    conv->Clear();
}
//-------------------------------------------------------------------------
bool tModelsConveyor::SendFile(const QString &_file_name)
{
    return true;
}
//-------------------------------------------------------------------------
bool tModelsConveyor::DeletingFile(const QString &_file_name, const bool _send)
{
    return true;
}
//-------------------------------------------------------------------------
void tModelsConveyor::CancelOperations()
{
    conv->CancelOperations();
}
//-------------------------------------------------------------------------
void tModelsConveyor::EndConveyor()
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Пакет команд выполнен"));
    MB.setWindowTitle(QString::fromUtf8("Пакет команд"));
    MB.exec();
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnDisconnect()
{
emit Disconnecting();
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnClose()//пробросить в форму
{

}
//-------------------------------------------------------------------------
void tModelsConveyor::OnAutorizStart()
{
    emit AutorizStart();
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnSetVisible(const bool vis)
{

}
//-------------------------------------------------------------------------
void tModelsConveyor::ErrorConveyor()
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Выполнение пакета команд прервано"));
    MB.setWindowTitle(QString::fromUtf8("Пакет команд"));
    MB.exec();
}
//-------------------------------------------------------------------------
void tModelsConveyor::Autorization(QString& _login, QString& _password)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SendAutorization");
    out << tr("SendAutorization");
    out << _login;
    out << _password;

    emit RunGui(block);
}
//-------------------------------------------------------------------------
void tModelsConveyor::OnListFiles()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GetListModels");
    out << tr("GetListModels");

    emit RunGui(block);
}
