#ifndef TDATABASEOP_H
#define TDATABASEOP_H

#include <QString>
#include <QtSql>


class tDatabaseOp
{
public:
    tDatabaseOp();
    ~tDatabaseOp();
    void RefreshModelsFiles();
    void ClearModels();
    void GetListModels(QStringList &_list);
    void GetLocalModelFiles(QString &_str, QStringList &_list);

    void SaveServerModelFiles(QByteArray &_block);
    void GetServerListFiles(const QString &_str, QStringList &_list);
    void GetServerListModels(QStringList &_list);
    QString GetLocalHash(const QString& _name_file_, bool& _sending, bool &_receiving);
    QString GetServerHash(const QString &name_file);
    void UpdateFileInfo(const QString& _file);
    void DeleteingFile(const QString &_file_name);
    QDateTime GetLastMod(const QString& _file_name);
    QDateTime RoundDateTime(const QDateTime &_dt) const;
    void PrepareSendFile(const QString& _file_name);

    void PrepareDeletingServer(const QString& _file_name);
    void PrepareReceiveFile(const QString& _file_name);
    void PrepareDeletingLocal(const QString& _file_name);
    void ClearlModConv();

    bool GetNextSendDelModel(QString &_name_model);
//    void GetSendModeles(QStringList &_list_models);
    void GetSendModelFiles(const QString &_name_model, QStringList& _list_files);
    void GetDeleteServerModelFiles(const QString &_name_model, QStringList& _list_files);
    void UpdateLastSynch(const QString& _file_name_, bool _server);
    bool GetNextReceiveDelModel(QString& _name_model);
    void GetReceiveModelFiles(const QString& _name_model_, QStringList& _list_files);
    void UpdateServerTable(const QString &_name);
    void GetDeleteLocalModelFiles(const QString& _name_model, QStringList& _list_files);
    void UpdateLastSynchDelServ(const QString& _file_name);

private:
    QSqlDatabase db;
    QString root;
    void SearchInfo(const QString &_folder);
    void AddModelFiles(const qlonglong _num, const QString _path, const QString _info_file, QDateTime &_list_date_time, QString &_list_files_hash, QString &_summ_list_hash);
    QString NormalizePathFiles(QString _path) const ;
    void UpdateModelFiles(const qlonglong _num, const QString _path, const QString _info_file, QString &_summ_list_hash);

    void CheckFile(const qlonglong _num, const QString &_path_file);
    void CheckInfoFiles(const qlonglong _num, const QString &_folder_mod);
    void UpdateLocalHash(const QString &_name_file, const QDateTime _disk_date_time, const QString &_hash);


};

#endif // TDATABASEOP_H
