#ifndef TMODELSCONVEYOR_H
#define TMODELSCONVEYOR_H

#include <QObject>
#include "ui_MainForm.h"
#include "tDatabaseOp.h"
#include "tConveyor.h"

class tModelsConveyor : public QObject
{
    Q_OBJECT
public:
    explicit tModelsConveyor(Ui::MainForm *_ui, QObject* _link, tDatabaseOp* _db_op, QObject *parent = 0);
//    void SetLink(QObject* _link){link=_link;}
    void SetDatabaseOperator(tDatabaseOp *_db_op){ db_op=_db_op;}
    void StartServer(const QString &_addr, const int _port);//Пробросить к конвейру
    void Clear();
    void SendFile(const QString &_file_name);
    void DeletingServerFile(const QString &_file_name);
    void CancelOperations();
    void Autorization(QString& _login, QString& _password);
    void OnListFiles();
    void StartSendDeleteFiles();
    void ReceiveFile(const QString& _file_name);
    void DeletingLocalFile(const QString& _file_name);
    void StartReceiveDeleteFiles();
    void CorrectLastSynch();

private:
    QObject *link;
    Ui::MainForm *ui;
    tDatabaseOp* db_op;
    tConveyor* conv;
    
signals:
    void NextCommand();
    void RunGui(QByteArray& block);//пробросить к конвейеру
    void Disconnecting();
    void AutorizStart();//Пробросить в форму
    void CloseMain();//Пробросить в форму
    void SetVisible(bool vis);//пробросить в форму
    void EndTransactions();

private slots:
    void EndConveyor();
    void OnDisconnect();
//    void OnClose();//пробросить в форму
    void OnAutorizStart();
//    void OnSetVisible(const bool vis);
    void ErrorConveyor();
};

#endif // TMODELSCONVEYOR_H
