#ifndef TMODELSCONVEYOR_H
#define TMODELSCONVEYOR_H

#include <QObject>
#include "tDatabaseOp.h"
#include "tConveyor.h"
#include "tLog.h"
#include "tModels.h"

class tModelsConveyor : public QObject
{
    Q_OBJECT
public:
    explicit tModelsConveyor( QObject* _link, tDatabaseOp* _db_op, QObject *parent = 0);
    ~tModelsConveyor();
    void SetDatabaseOperator(tDatabaseOp *_db_op){ db_op=_db_op;}
    void StartServer(const QString &_addr, const int _port);
    void Clear();
    void SendFile(const QString &_file_name);
    void DeletingServerFile(const QString &_file_name);
    void CancelOperations();
    void Autorization(const QString &_login, const QString &_password);
    void OnListFiles();
    void StartSendDeleteFiles(const int _max_model, bool Ok);
    void ReceiveFile(const QString& _file_name);
    void DeletingLocalFile(const QString& _file_name);
    void StartReceiveDeleteFiles(const QString &_root, int _custom_copy, int max_model, bool Ok);
    void CorrectLastSynch(const bool _server);
    bool GetIsTransaction()
    {
        return Transaction;
    }

    void SetTransactionFlag(const bool _flag);
    void MarkLastTables(const bool _send, const QString &_user_login);

    void SendLoginPassword(const QString &_login, const QString &_password, const int _row, const bool _new_user);
    void SendDeleteLogin(const int _row);

    void ReceiveLoginsTable();
    void SavePermissionsToServer(const qlonglong _num_login);
    void SetLogin(const QString& _user_login);

    void ClearAllList();
    void SaveLoginWritable(const QStandardItemModel *_model, const int _row);
    bool error_transaction;
    void ReceivingModels(QList<tServerModel> &_models, QString _root);

private:
    QObject *link;
//    Ui::MainForm *ui;
    tDatabaseOp* db_op;
    tConveyor* conv;
    bool Transaction;
    bool send;
    QStringList all_files;

    QString l;
    tLog log;
    QString user_login;
    QString root_folder;
    int mod_custom_copy;
    int count_models;
    int max_model;
    QList<tServerModel> auto_load_models;
    void StartAutoReceiveFiles(const QString &root_folder);
    int number_auto_model;

signals:
    void NextCommand();
    void RunGui(QByteArray& block);
//    void Disconnecting();
    void AutorizStart();
    void CloseMain();
    void SetVisible(bool vis);
    void EndTransactions();
    void EndTransactionsMain();
    void Disconnecting();
    void SendModels();

    void SignalCountFiles(int count_files);

public slots:
    void OnRunGui(QByteArray& _block);

private slots:
    void EndConveyor(bool Ok);
    void OnDisconnect();
    void OnAutorizStart();
    void ErrorConveyor();
    void OnEndTransactions();
    void OnSendModels();




};

#endif // TMODELSCONVEYOR_H

