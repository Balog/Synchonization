#ifndef TDATABASEOP_H
#define TDATABASEOP_H

#include <QString>
#include <QtSql>
#include "tLog.h"


class tDatabaseOp
{
public:
    tDatabaseOp(const QSqlDatabase &_db);
    ~tDatabaseOp();
    void RefreshModelsFiles();
    void ClearModels();
    void GetListModels(QDataStream &_out);
    void GetLocalModelFiles(const QString &_str, QStringList &_list);
    QString GetHash(const QString &name_file) const;
    QDateTime GetLastMod(const QString &name_file) const;
    QDateTime RoundDateTime(const QDateTime &_dt) const;
    void Update_LastMod_Hash(const QString &_file_name, const QDateTime &_last_mod, const QString &_hash);
    QString SaveLoginPass(QString& _login, QString& _pass, bool _new_user, qlonglong &num_log, int _row);

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
};

#endif // TDATABASEOP_H
