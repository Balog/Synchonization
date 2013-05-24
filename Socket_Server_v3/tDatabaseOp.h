#ifndef TDATABASEOP_H
#define TDATABASEOP_H

#include <QString>
#include <QtSql>
#include "tLog.h"

struct tFileList
{
    QString file_name;
    QString client_hash;
    QString server_hash;
};

class tDatabaseOp
{
public:
    tDatabaseOp(const QSqlDatabase &_db);
    ~tDatabaseOp();
    void RefreshModelsFiles();
    void ClearModels();
    void GetListModels(QDataStream &_out, QString &_login);
    void GetLocalModelFiles(const QString &_str, QStringList &_list);
    QString GetHash(const QString &name_file) const;
    QDateTime GetLastMod(const QString &name_file) const;
    QDateTime RoundDateTime(const QDateTime &_dt) const;
    void Update_LastMod_Hash(const QString &_file_name, const QDateTime &_last_mod, const QString &_hash);
    QString SaveLoginPass(QString& _login, QString& _pass, bool _new_user, qlonglong &num_log);
    QString DeleteLogin(qlonglong num_login);
    void SendLoginTable(QDataStream &_out);
    void SendReadPermissions(QDataStream &_out);
    bool VerifyAutorization(QString& _login, QString& _password);
    void SavePermissions(QByteArray _block);
    qlonglong GetNumLogin(QString &_login);
    void SaveLoginWritable(int row, bool writ);
    bool VerifyFile(QString& name_file, QString& server_hash);
    void CorrectReadPermission(QString& _trans_model, QString& _login);
    void CorrectWrittenWho(QList<tFileList> list, QString& login);

private:
    QSqlDatabase db;
    QString root;
    void SearchInfo(const QString &_folder);
    void AddModelFiles(const qlonglong _num, const QString _path, QString _info_file, QDateTime &_list_date_time, QString &_list_files_hash, QString &_summ_list_hash);
    QString NormalizePathFiles(QString _path) const;
    void UpdateModelFiles(const qlonglong _num, const QString _path, QString _info_file, QString &_summ_list_hash);

    void CheckFile(const qlonglong _num, const QString &_path_file);
    void CheckInfoFiles(const qlonglong _num, const QString &_folder_mod);

    tLog log;
    bool IsNoDelete(QString& _login);

};

#endif // TDATABASEOP_H
