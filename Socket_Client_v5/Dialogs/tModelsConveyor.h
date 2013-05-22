#ifndef TMODELSCONVEYOR_H
#define TMODELSCONVEYOR_H

#include <QObject>
#include "ui_MainForm.h"
#include "tDatabaseOp.h"
#include "tConveyor.h"
#include "tLog.h"

class tModelsConveyor : public QObject
{
    Q_OBJECT
public:
    explicit tModelsConveyor(Ui::MainForm *_ui, QObject* _link, tDatabaseOp* _db_op, QObject *parent = 0);
    ~tModelsConveyor();
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
    void CorrectLastSynch(bool _server);
    bool GetIsTransaction()
    {
        return Transaction;
    }

    void SetTransactionFlag(bool _flag);
    void MarkLastTables(bool _send, const QString &user_login);

    void SendLoginPassword(QString &_login, QString &_password, int _row, bool _new_user);
    void SendDeleteLogin(int _row);

    void ReceiveLoginsTable();
    void SavePermissionsToServer(qlonglong _num_login);
    void SetLogin(const QString& _user_login);

    void ClearAllList();
    void SaveLoginWritable(QStandardItemModel* model, int _row);

private:
    QObject *link;
    Ui::MainForm *ui;
    tDatabaseOp* db_op;
    tConveyor* conv;
    bool Transaction;
    bool send;
    QStringList all_files;

    QString l;
    tLog log;
    QString user_login;
    
signals:
    void NextCommand();
    void RunGui(QByteArray& block);//пробросить к конвейеру
    void Disconnecting();
    void AutorizStart();//Пробросить в форму
    void CloseMain();//Пробросить в форму
    void SetVisible(bool vis);//пробросить в форму
    void EndTransactions();
    void EndTransactionsMain();

private slots:
    void EndConveyor();
    void OnDisconnect();
    void OnAutorizStart();
    void ErrorConveyor();
    void OnEndTransactions();



};

#endif // TMODELSCONVEYOR_H
