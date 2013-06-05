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
    
public slots:
    void OnFindServer(bool ok);
    void OnSendAutorization(QString& _login, QString& _password, bool _mod_folder);
    void OnEndUpdatingFromServer(QList<CompareTableRec> _list_compare, bool _rebuild);

private slots:
    void OnStartAutorizForm();
    void setValue(int _value);
    void OnEndTransactions();
    void OnRebuildTrees(QList<CompareTableRec> _list);
//    void OnretEndUpdateServerModel(bool _rebuild);

    
};

#endif // TEXPORTMAIN_H
