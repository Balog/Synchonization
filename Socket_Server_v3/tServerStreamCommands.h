#ifndef TSERVERSTREAMCOMMANDS_H
#define TSERVERSTREAMCOMMANDS_H
#include "tAbstractStreamCommand.h"
#include "tFileStreamCommand.h"
#include "tSettings.h"
#include <QDebug>
#include <QDateTime>
#include "Windows.h"
#include "tCalcHash.h"
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QList>
#include "tDatabaseStreamCommand.h"

FILETIME QDateTimeToFILETIME(QDateTime &dt);

class tSendStreamFile : public tFileStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream&, QDataStream& _out);
    virtual void SendErrorReport(QDataStream &_in);
    virtual void SendReport(QDataStream &) {}
    virtual void ExternalExit(){}

private:
    bool cont;


};
inline tAbstractStreamCommand* Create_tSendStreamFile()
{
    return new tSendStreamFile;
}
//*****************************************************************
class tReceiveStreamFile : public tFileStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_in, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &_in);
    virtual void SendReport(QDataStream &_out);
    virtual void ExternalExit();
    ~tReceiveStreamFile();

private:
    bool SetAttributeFile(const QString &_root, const QString &_file_name, const bool _hidden, const QDateTime _create_date_time, const QDateTime _last_modified, const int _permission, const QString &_hash);
    FILETIME QDateTimeToFILETIME(const QDateTime &dt) const ;

    bool enabled;
    qint64 read_size;
    bool error_detected;
    bool hidden;
    QDateTime create_date_time;
    QDateTime last_modified;
    int permission;
    QString hash;
    FILETIME cr_dt;
    FILETIME last_dt;
    HANDLE handle;
    tCalcHash *calc_hash;
    QString error_reason;
    QString login;
};
inline tAbstractStreamCommand* Create_tReceiveStreamFile()
{
    return new tReceiveStreamFile;
}
//*****************************************************************
class tError : public tAbstractStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &) {return true;}
    virtual bool ExeCommand(QDataStream &, QDataStream &) {return true;}
    virtual void SendErrorReport(QDataStream &_in);
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {}

private:


};
inline tAbstractStreamCommand* Create_tError()
{
    return new tError;
}
//*****************************************************************
class tConnectionReport : public tAbstractStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_in, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {}

private:


};
inline tAbstractStreamCommand* Create_tConnectionReport()
{
    return new tConnectionReport;
}
//*****************************************************************
class tPrepareReceiveFile : public tFileStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &_in);
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

};
inline tAbstractStreamCommand* Create_tPrepareReceiveFile()
{
    return new tPrepareReceiveFile;
}
//*****************************************************************
class tAutorization : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    QString login;
    QString password;
};
inline tAbstractStreamCommand* Create_tAutorization()
{
    return new tAutorization;
}
//*****************************************************************
class tStartTransaction : public tAbstractStreamCommand

{
    Q_OBJECT

    struct tFileList
    {
        QString file_name;
        QString client_hash;
        QString server_hash;
    };

public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &_out);
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    bool removeFolder(const QDir &dir) const ;
    void ClearTempFolder();
    bool VerifyCollisions();

    QList<tFileList> file_list;
    QString root;
    QString login;
    QString temp_path;
    bool send_mode;
    QDir dir;

};
inline tAbstractStreamCommand* Create_tStartTransaction()
{
    return new tStartTransaction;
}
//*****************************************************************
class tCommitTransaction : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    bool FolderOperation(const QDir &_dir, const int _mode);
    void Move(const QString &_entry_abs_path, const QString &_new_abs_path, bool &_stopped, QString &_error_file);
    void Verify(const QString &_new_abs_path, bool &_stopped, QString &_error_file);
    bool Delete(const QString &_new_abs_path, QString &_error_file);
    void SendError(QDataStream &_out);
    void SendOk(QDataStream &_out);
    bool DeleteEmptyFolders(const QString &_root);

    QString temp;
    QString root;
    bool stopped;
    QString error_file;

    void VerifyReplacedFiles();
    void VerifyDeletedFiles();

private:
    struct tFileList
    {
        QString file_name;
        QString client_hash;
        QString server_hash;
    };
//    QString model_file;
    QList<tFileList> file_list;
    int send_mode;
};
inline tAbstractStreamCommand* Create_tCommitTransaction()
{
    return new tCommitTransaction;
}
//*****************************************************************
class tCancelTransaction : public tAbstractStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    void ClearTempFolder();
    bool removeFolder(const QDir &_dir);

    QString temp;
    QString login;
    QDir dir;
};
inline tAbstractStreamCommand* Create_tCancelTransactionn()
{
    return new tCancelTransaction;
}
//*****************************************************************
class tDeleteFile : public tAbstractStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_in, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    struct tFileList
    {
        QString file_name;
        QString server_hash;
    };
    QList<tFileList> del_list;
};
inline tAbstractStreamCommand* Create_tDeleteFile()
{
    return new tDeleteFile;
}
//*****************************************************************
class tGetFileList : public tAbstractStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    void CreateFileList(const QString &start_folder, QStringList &list);
    void SearchFiles(const QDir &dir, QStringList &list);
};
inline tAbstractStreamCommand* Create_tGetFileList()
{
    return new tGetFileList;
}
//*****************************************************************
class tGetListModels : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
//    void CreateFileList(QStringList &_list);

};
inline tAbstractStreamCommand* Create_tGetListModels()
{
    return new tGetListModels;
}
//*****************************************************************
class tSaveLoginPassword : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &);
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    QString login;
    QString password;
    bool new_user;
    qlonglong num_log;
    int row;

};
inline tAbstractStreamCommand* Create_tSaveLoginPassword()
{
    return new tSaveLoginPassword;
}
//*****************************************************************
class tDeleteLogin : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &);
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    qlonglong num_login;

};
inline tAbstractStreamCommand* Create_tDeleteLogin()
{
    return new tDeleteLogin;
}
//*****************************************************************
class tSendLoginsTable : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    qlonglong num_login;

};
inline tAbstractStreamCommand* Create_tSendLoginsTable()
{
    return new tSendLoginsTable;
}
//*****************************************************************
class tSendReadPermissions : public tDatabaseStreamCommand

{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &, QDataStream &_out);
    virtual void SendErrorReport(QDataStream &){}
    virtual void SendReport(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    qlonglong num_login;

};
inline tAbstractStreamCommand* Create_tSendReadPermissions()
{
    return new tSendReadPermissions;
}
//*****************************************************************
#endif // TSERVERSTREAMCOMMANDS_H
