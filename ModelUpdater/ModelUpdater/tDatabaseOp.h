#ifndef TDATABASEOP_H
#define TDATABASEOP_H

#include <QString>
#include <QtSql>
#include "tLog.h"
#include <QStandardItemModel>

struct tFile
{
    bool Local; // 1 - локальный файл, 0 - серверный файл
    qlonglong num;
    QString file_name;
    qint64 size;
    QString last_mod;
    int IsFounded;
    qlonglong NumLoginMod;

} ;

struct CompareTableRec
{
    qlonglong num;
    qlonglong model_local;
    qlonglong model_server;
    QString mod_struct;
    int result;
    QList<tFile> file;
    bool read_choice;
    bool write_choice;
};

//**********************************************************************
class tDatabaseOp
{
public:
    tDatabaseOp();
    ~tDatabaseOp();

    void RefreshModelsFiles();
    void ClearModels();
    void GetListModels(QStringList &_list);
    void GetLocalModelFiles(const QString &_str, QStringList &_list);

    void SaveServerModelFiles(QByteArray &_block);
    void GetServerListFiles(const QString &_str, QStringList &_list);
    void GetServerListFiles(const qlonglong _num_server_model, QStringList &_list);
    void GetServerListModels(QStringList &_list);
    QString GetLocalHash(const QString& _name_file_, bool& _sending, bool &_receiving);
    QString GetServerHash(const QString &_name_file);
    void DeleteingFile(const QString &_file_name);
    QDateTime GetLastMod(const QString& _file_name);
    QDateTime RoundDateTime(const QDateTime &_dt) const;
    void PrepareSendFile(const QString& _file_name);

    void PrepareDeletingServer(const QString& _file_name);
    void PrepareReceiveFile(const QString& _file_name);
    void PrepareDeletingLocal(const QString& _file_name);
    void ClearlModConv();

    bool GetNextSendDelModel(QString &_name_model, int &count);
    void GetSendModelFiles(const QString &_name_model, QStringList& _list_files);
    void GetDeleteServerModelFiles(const QString &_name_model, QStringList& _list_files);

    void PrepareUpdateLastSynch(const bool _send, const QString &_login);
    void UpdateLastSynchMark(const QString& _file_name_, const bool _server, const QString &_user_login);
    void ExecUpdateLastSynch(const bool _server, const QString &_user_login);

    bool GetNextReceiveDelModel(QString& _name_model, int &_count);
    void GetReceiveModelFiles(const QString& _name_model_, QStringList& _list_files);
    void UpdateServerTable(const QString &_name);
    void GetDeleteLocalModelFiles(const QString& _name_model, QStringList& _list_files);

    void SaveLoginPassword(const QString &_login, const QString &_password, const bool _new_user, const qlonglong _s_num);
    QStringList GetLoginsList() const;

    qlonglong GetNumLogin(const QString &_login) const;
    qlonglong GetNumLogin(const int _row) const;

    void DeleteLogin(const qlonglong _num_log);
    void UpdateLogins(QByteArray &_block);

    bool VerPassword(const QString &login, const QString &_pass);


    void ResetFoundModelAdmin();
    bool NextModelAdmin() const;
    QStringList NextStructListModelAdmin(const QString &_login, bool &_read, qlonglong &_server_num) const;
    void UpdateModelRead(QByteArray &_block);
    void SaveReadPermission(const QString &_login, const qlonglong _mod_num, const bool _state);
    void SavePermissionsToServer(const qlonglong _num_log, QByteArray &_block);

    bool VerifyUserFolders(const QString& _login, const QString &_project_folder, const QString &_temp_folder, QString &_message) const;
    void SaveFoldersToLoginsTable(const QString& _login, const QString& _project_folder, const QString& _temp_folder);
    void SaveFoldersToSettings(const QString& _userlogin);

    void VerifyLastTable(const QString& _login);
    void WriteToCompareTablesToTree(const QString& _login);
    QList<CompareTableRec> AnalyzeCompareAll();
    void AddFilesToModelsStruct(QList<CompareTableRec> &comp_models);

    void ActualiseModel(const QString &_login, const qlonglong _num_model, const bool _from_server);
    void GetLoginsModel(QStandardItemModel *model);
    void SaveLoginsWritable(const QStandardItemModel *_model, const int _row, bool &_check);
    void GetPermissionsUser(const QString &_user_login, bool &_is_admin_user, bool &_is_writable_user);
    int GetCountRecDelModels();
    int GetCountSendDelModels();

    void GetModelInfo(const qlonglong _loc_num, QString& _title_model, QString &_description, QList<tFile> &_files_model, QStringList& _previews);
    void SaveDescription(const qlonglong _num, const QString &_text, QString &_file_name);
    void GetServerListPreviews(const qlonglong _server_num_model, QStringList &_list);
    void GetServerModelInfo(const qlonglong _serv_num, QString& _title_model, QString &_description, QList<tFile> &_files_model);
    QString GetServerModelPath(const qlonglong _num_server) const;
    void UpdateInfoData(const qlonglong _num_model, const QString &_file_name, const QString &_hash, const QDateTime &_last_mod);
    QString LoginFromNum(const qlonglong _num_login) const;
    QString VerifyCustomCopyPath(const QString& path) const;

private:
    typedef enum {Local, Last, Server} tTableLevel;
    typedef enum {no_changes, change_local, change_server, new_local, new_server, conflict} CompareRes;

    QSqlDatabase db;
    QString root;
    void SearchInfo(const QString &_folder);
    void AddModelFiles(const qlonglong _num, const QString _path, const QString _info_file, QDateTime &_list_date_time, QString &_list_files_hash, QString &_summ_list_hash);
    QString NormalizePathFiles(QString _path) const ;
    void UpdateModelFiles(const qlonglong _num, const QString _path, const QString _info_file, QString &_summ_list_hash);

    void CheckFile(const qlonglong _num, const QString &_path_file);
    void CheckInfoFiles(const qlonglong _num, const QString &_folder_mod);
    void UpdateLocalHash(const QString &_name_file, const QDateTime _disk_date_time, const QString &_hash);

    QString l;
    tLog log;

    void MarkLastModel(const qlonglong num_login, const QString& m_struct);
    void UpdateCompareTable(const qlonglong _num, const QString& _m_struct, const QString& _summ_hash, const tTableLevel _level);
    void AnalyzeCompare(const CompareRes _res);

};

#endif // TDATABASEOP_H
