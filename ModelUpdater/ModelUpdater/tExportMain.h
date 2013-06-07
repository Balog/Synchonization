#ifndef TEXPORTMAIN_H
#define TEXPORTMAIN_H

#include <QObject>
#include "MainModule.h"
#include "tZastModule.h"
#include "tDatabaseOp.h"

class Q_DECL_EXPORT tExportMain : public QObject
{
    Q_OBJECT
public:
    explicit tExportMain(QObject *parent = 0);
    ~tExportMain();
//    tDatabaseOp *GetDatabase();
    QString GetTemp();
    void GetModelInfo(const qlonglong _loc_num, QString& _title_model, QString &_description, QList<tFile> &_files_model, QStringList& _previews);
    QString LoginFromNum(const qlonglong _num_login) const;
    QString GetServerModelPath(const qlonglong _num_server) const;
    void GetServerModelInfo(const qlonglong _serv_num, QString& _title_model, QString &_description, QList<tFile> &_files_model);
    void SaveDescription(const qlonglong _num, const QString &_text, QString &_file_name);
    void UpdateInfoData(const qlonglong _num_model, const QString &_file_name, const QString &_hash, const QDateTime &_last_mod);
    QString GetRoot();
    void OnListFilesLocal();
    void WriteToCompareTablesToTree(const QString& _login);
    QList<CompareTableRec> AnalyzeCompareAll();
    void AddFilesToModelsStruct(QList<CompareTableRec> &comp_models);
    void SetTransactionFlag(const bool _flag);
    void ClearConveyor();
    void DeletingLocalFile(const QString& _file_name);
    void ReceiveFile(const QString& _file_name);
    void GetServerListPreviews(const qlonglong _server_num_model, QStringList &_list);
    void GetServerListFiles(const qlonglong _num_server_model, QStringList &_list);
    int GetCountRecDelModels();
    void StartReceiveDeleteFiles(const QString &_root, int _custom_copy, int max_model);
    void DeletingServerFile(const QString &_file_name);
    void SendFile(const QString &_file_name);
    int GetCountSendDelModels();
    void StartSendDeleteFiles(const int _max_model);
    void ActualiseModel(const QString &_login, const qlonglong _num_model, const bool _from_server);
    QString VerifyCustomCopyPath(const QString& path) const;
    void RunGui(QByteArray& block);
    void Disconnecting();
    void SetServerParameters(const QString &_addr, const int _port);
    void StartServer();
    void GetServerParameters(QString &_addr, int& _port);
    void ReStartServer();
    void GetFolderParameters(const QString& login, QString& roor_folder, QString& temp_folder, QString& mess);
    bool VerifyUserFolders(QString& _login, QString& _pr_folder, QString& _temp_folder, QString& _message);
    void SaveFoldersToLoginsTable(const QString &_login, const QString &_pr_folder, const QString &_temp_folder);
    void SaveFoldersToSettings(const QString &_user_login);
    void GetPermissionsUser(const QString& _login, bool &_is_admin_user, bool &_is_writable_user);
    void RefreshModelsFiles();
    bool removeFolder(const QDir & _dir, const bool _del_dir);
    void SendDeleteLogin(int _num);
    void SaveLoginWritable(const QStandardItemModel *_model, const int _row);
    void GetLoginsModel(QStandardItemModel *model);
    bool VerPassword(const QString &login, const QString &_pass);
    void SavePermissionsToServer(const qlonglong _num_login);
    qlonglong GetNumLogin(const int _row) const;
    qlonglong GetNumLogin(const QString &_login) const;
    void SaveReadPermission(const QString &_login, const qlonglong _mod_num, const bool _state);
    void ResetFoundModelAdmin();
    bool NextModelAdmin() const;
    QStringList NextStructListModelAdmin(const QString &_login, bool &_read, qlonglong &_server_num) const;
    void SendLoginPassword(const QString &_login, const QString &_password, const int _row, const bool _new_user);

private:
    MainModule* MModule;
//    tZastModule* zast_mod;
    void StartFindServer();
    
signals:
    void FindServer(bool ok);
    void StartAutorizForm();
    void SendAutorization(QString& _login, QString& _password, bool _mod_folder);
    void EndUpdatingFromServer(QList<CompareTableRec> _list_compare, bool _rebuild);
    void SignalCountFiles(int _value);
    void EndTransactions();
    void retEndUpdateServerModel(bool _rebuild);
    void Disconnect();
    void ErrorUserFolders(QString& user_login, QString& message);
    void Update_Logins();
    
public slots:
    void OnFindServer(bool ok);
    void OnSendAutorization(QString& _login, QString& _password, bool _mod_folder);
    void OnEndUpdatingFromServer(QList<CompareTableRec> _list_compare, bool _rebuild);
    void OnUpdate_Logins();

private slots:
    void OnStartAutorizForm();
    void setValue(int _value);
    void OnEndTransactions();
    void OnRebuildTrees(QList<CompareTableRec> _list);
    void OnDisconnectingFromServer();
    void OnErrorUserFolders(QString& _login,QString& _mess);

//    void OnretEndUpdateServerModel(bool _rebuild);

    
};

#endif // TEXPORTMAIN_H
