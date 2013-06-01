#ifndef TCONVEYOR_H
#define TCONVEYOR_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include "ClassFactory.h"
#include "tAbstractGuiCommand.h"
#include "tClientGuiCommand.h"
#include "tClientThread.h"
#include <QList>
#include "tSettings.h"
#include <QDir>
#include "tDatabaseOp.h"

#include <vector>
#include "tLog.h"

using namespace std;

struct tFileList
{
    QString file_name;
    QString client_hash;
    QString server_hash;
};


class tConveyor : public QObject
{
    Q_OBJECT
public:
    explicit tConveyor(QObject* _link, tDatabaseOp *_db_op, QObject *parent);
    ~tConveyor();
    void StartServer(const QString &_addr, const int _port);

    void AddCommand(const QByteArray _block);

    void Clear();

    bool SendFile(const QString &_file_name, QStringList &_all_files);
    bool ReceiveFile(const QString &_file_name, QStringList &_all_files);
    bool DeletingFile(const QString &_file_name, QStringList &_all_files, const bool _send);

    void AddCommitTransaction(const bool _send, const QString &_root, const bool _custom_copy);
    void AddCommitTransactionDel();

    bool AddSendCommand();
    void AddDelCommand();
    bool AddReceiveCommand(const QString &_root);

    void AddStartTransaction(const bool _send);
    void StartExecution();

    void SearchInfo(QString &_folder, QSqlQuery StructModels);

    void CancelOperations();

    void GetListServerModels();

    void CorrectLastSynch(const QStringList &_all_files, const bool _server);
    void GetServerModels();
    void ClearTempFolder();
    void SetLogin(const QString& _user_login);

private:
    QObject *link;
//    Ui::MainForm *ui;
    vector <QByteArray> v_conv;
    typedef vector <QByteArray> t_conv;
    t_conv::const_iterator it_conv;

    tVfactory<tAbstractGuiCommand, QString> gui_vf;
    tAbstractGuiCommand* gui_comm;

    tClientThread *client_th;

    QList<tFileList>file_list;
    QList<tFileList>file_list1;
    QList<tFileList>file_list_del;

    QString root;
    QString temp;

    int send_mode;  //1 -- Send
    //2 -- Receive
    //0 -- ничего не производилось


    bool removeFolder(const QDir &_dir, const bool _del_dir);
    bool FolderOperation(const QDir &_dir, const int _mode);
    void Move(const QString &_entry_abs_path, const QString &_new_abs_path, bool &_stopped, QString &_error_file);
    void Verify(const QString &_new_abs_path, bool &_stopped, QString &_error_file);
    bool Delete(const QString &_new_abs_path, QString &_error_file) const ;
    bool DeleteEmptyFolders(const QString &_root) const;

    bool stopped;
    QString error_file;
    QStringListModel *slm_list;

    tDatabaseOp* db_op;
    QString model_file;


    void AddModelFiles(qlonglong Num, QString Path, QString InfoFile, QDateTime &_list_date_time, QString &list_files_hash, QString &summ_list_hash);
    QString NormalizePathFiles(QString Path);
    void VerifyReplacedFiles(bool _custom_copy);
    void VerifyDeletedFiles();

    QString l;
    tLog log;
    QString user_login;

signals:
    void EndCommands();
    void Disconnect();
    void CloseMain();
    void AutorizStart();
    void SetVisible(bool vis);
    void DisconnectFromServer();
    void ErrorCommands();
    void EndTransactions();
    void EndConveyor();


public slots:
    void NextCommand();
    void OnRunGuiCommand(QByteArray& _block);
    void OnCommand(QByteArray _block);
    void OnDisconnect();
    void OnStartStop(const bool _res);
    void OnStart(const bool _res);
    void OnDisconnecting();
    void VerifyMoveDelete(QString &_root_folder, bool _custom_copy);
    void OnEndTransactions();
    void OnEndConveyor();
    void OnEndCommand();
};

#endif // TCONVEYOR_H
