#ifndef TCLIENTSTREAMCOMMANDS_H
#define TCLIENTSTREAMCOMMANDS_H
#include <QDebug>
#include "tAbstractStreamCommand.h"
#include "tFileStreamCommand.h"
#include "tSettings.h"
#include "tCalcHash.h"
#include <QDateTime>
#include "Windows.h"
#include <QList>


//****************************************************************
class tStreamReceiveFile : public tFileStreamCommand
{
    Q_OBJECT
public:
    ~tStreamReceiveFile();
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &in);
    virtual void ProcessError(QDataStream &);
    virtual void ExternalExit() {}

private:
    bool enabled;
    qint64 read_size;
    QString error_reason;


    bool hidden;
    QDateTime create_date_time;
    QDateTime last_modified;
    int permission;
    QString hash;
    FILETIME cr_dt;
    FILETIME last_dt;
    HANDLE handle;
    tCalcHash *calc_hash;
    QString ErrorReason;

    bool SetAttributeFile(const QString &_file_name, const bool _hidden, const QDateTime& _create_date_time, const QDateTime& _last_modified, const int _permission, const QString &_hash);
    FILETIME QDateTimeToFILETIME(const QDateTime &_dt) const;
};
inline tAbstractStreamCommand* Create_tReceiveFile()
{
    return new tStreamReceiveFile;
}
//****************************************************************
class tPrepareReceiveFile : public tFileStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_in);
    virtual void ProcessError(QDataStream &);
    virtual void ExternalExit();
};
inline tAbstractStreamCommand* Create_tPrepareReceiveFile()
{
    return new tPrepareReceiveFile;
}
//****************************************************************
class tSendFile : public tFileStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {}
private:
    bool Continue;
    int num_err;
    QString error;
    QString detail;
    QString client_detail;
    QString Hash;
};
inline tAbstractStreamCommand* Create_tSendFile()
{
    return new tSendFile;
}
//****************************************************************
class tReportSendFile : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {emit EndCommand();}
};
inline tAbstractStreamCommand* Create_tReportSendFile()
{
    return new tReportSendFile;
}
//****************************************************************
class tProcessingError : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &) {return true;}
    virtual bool ExeCommand(QDataStream &) {return true;}
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {}
};
inline tAbstractStreamCommand* Create_tProcessingError()
{
    return new tProcessingError;
}
//****************************************************************
class tConnectConfirmReport : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &) {return true;}
    virtual bool ExeCommand(QDataStream &_in) ;
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
};
inline tAbstractStreamCommand* Create_tConnectConfirmReport()
{
    return new tConnectConfirmReport;
}
//****************************************************************
class tStreamPrepareSendFile : public tFileStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out) ;
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
};
inline tAbstractStreamCommand* Create_tStreamPrepareSendFile()
{
    return new tStreamPrepareSendFile;
}
//****************************************************************
class tStreamReportPrepareSendFile : public tFileStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &) ;
    virtual void ProcessError(QDataStream &_out);
    virtual void ExternalExit() {emit EndCommand();}


};
inline tAbstractStreamCommand* Create_tStreamReportPrepareSendFile()
{
    return new tStreamReportPrepareSendFile;
}
//****************************************************************
class tStreamSendAutorization : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out) ;
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {emit EndCommand();}

private:
    QString login;
    QString password;
};
inline tAbstractStreamCommand* Create_tStreamSendAutorization()
{
    return new tStreamSendAutorization;
}
//****************************************************************
class tStreamReportAutorization : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &) ;
    virtual void ProcessError(QDataStream &);
    virtual void ExternalExit() {emit EndCommand();}

private:

};
inline tAbstractStreamCommand* Create_tStreamReportAutorization()
{
    return new tStreamReportAutorization;
}
//****************************************************************
class tStreamStartTransaction : public tAbstractStreamCommand
{
    Q_OBJECT

    struct tFileList
    {
        QString file_name;
        QString client_hash;
        QString server_hash;
    };

    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QList<tFileList> file_list;
    bool send;
};
inline tAbstractStreamCommand* Create_tStreamStartTransaction()
{
    return new tStreamStartTransaction;
}
//****************************************************************
class tStreamReportStartTransaction : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {emit EndCommand();}

private:

};
inline tAbstractStreamCommand* Create_tStreamReportStartTransaction()
{
    return new tStreamReportStartTransaction;
}
//****************************************************************
class tStreamCommitTransaction : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    struct tFileList
    {
        QString file_name;
        QString client_hash;
        QString server_hash;
    };
    QList<tFileList>file_list;
    int send;
};
inline tAbstractStreamCommand* Create_tStreamCommitTransaction()
{
    return new tStreamCommitTransaction;
}
//****************************************************************
class tStreamReportCommitTransaction : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {emit EndCommand();}

private:

};
inline tAbstractStreamCommand* Create_tStreamReportCommitTransaction()
{
    return new tStreamReportCommitTransaction;
}
//****************************************************************
class tStreamCancelTransaction : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &);
    virtual void ExternalExit() {emit EndCommand();}

private:

};
inline tAbstractStreamCommand* Create_tStreamCancelTransaction()
{
    return new tStreamCancelTransaction;
}
//****************************************************************
class tStreamReportCancelTransaction : public tAbstractStreamCommand
{
    Q_OBJECT
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:

};
inline tAbstractStreamCommand* Create_tStreamReportCancelTransaction()
{
    return new tStreamReportCancelTransaction;
}
//****************************************************************
class tDeleteFile : public tFileStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    bool Continue;

};
inline tAbstractStreamCommand* Create_tDeleteFile()
{
    return new tDeleteFile;
}
//****************************************************************
class tReportDeleteFile : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
};
inline tAbstractStreamCommand* Create_tReportDeleteFile()
{
    return new tReportDeleteFile;
}
//****************************************************************
class tGetListFiles : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
};
inline tAbstractStreamCommand* Create_tGetListFiles()
{
    return new tGetListFiles;
}
//****************************************************************
class tReportGetListFiles : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    int num_files;
};
inline tAbstractStreamCommand* Create_tReportGetListFiles()
{
    return new tReportGetListFiles;
}
//****************************************************************
class tGetServerListModels : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
};
inline tAbstractStreamCommand* Create_tGetServerListModels()
{
    return new tGetServerListModels;
}
//****************************************************************
class tReportGetListServerModels : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QByteArray block1;
};
inline tAbstractStreamCommand* Create_tReportGetListServerModels()
{
    return new tReportGetListServerModels;
}
//****************************************************************
class tStreamSaveLoginPassword : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QString login;
    QString password;
    bool new_user;
    qlonglong num_log;
    int row;
};
inline tAbstractStreamCommand* Create_tStreamSaveLoginPassword()
{
    return new tStreamSaveLoginPassword;
}
//****************************************************************
class tStreamReportSaveLoginPassword : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {emit EndCommand();}

private:
    qlonglong s_num;
};
inline tAbstractStreamCommand* Create_tStreamReportSaveLoginPassword()
{
    return new tStreamReportSaveLoginPassword;
}
//****************************************************************
class tStreamDeleteLogin : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    qlonglong num_login;
};
inline tAbstractStreamCommand* Create_tStreamDeleteLogin()
{
    return new tStreamDeleteLogin;
}
//****************************************************************
class tStreamReportDeleteLogin : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &_in);
    virtual void ExternalExit() {emit EndCommand();}

private:
    qlonglong s_num;
};
inline tAbstractStreamCommand* Create_tStreamReportDeleteLogin()
{
    return new tStreamReportDeleteLogin;
}
//****************************************************************
class tGetLoginsTable : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

};
inline tAbstractStreamCommand* Create_tGetLoginsTable()
{
    return new tGetLoginsTable;
}
//****************************************************************

class tStreamReportReceiveLoginsTable : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QByteArray block1;
};
inline tAbstractStreamCommand* Create_tStreamReportReceiveLoginsTable()
{
    return new tStreamReportReceiveLoginsTable;
}
//****************************************************************
class tStreamReceiveReadPermissions : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

};
inline tAbstractStreamCommand* Create_tStreamReceiveReadPermissions()
{
    return new tStreamReceiveReadPermissions;
}
//****************************************************************
class tStreamReportReceiveReadPermissions : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QByteArray block1;
};
inline tAbstractStreamCommand* Create_tStreamReportReceiveReadPermissions()
{
    return new tStreamReportReceiveReadPermissions;
}
//****************************************************************
class tStreamSavePermissions : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    QByteArray block1;
};
inline tAbstractStreamCommand* Create_tStreamSavePermissions()
{
    return new tStreamSavePermissions;
}
//****************************************************************
class tStreamReportSavePermissions : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QByteArray block1;
};
inline tAbstractStreamCommand* Create_tStreamReportSavePermissions()
{
    return new tStreamReportSavePermissions;
}
//****************************************************************
class tStreamSaveLoginWritable : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &_in);
    virtual bool ExeCommand(QDataStream &_out);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}
private:
    int row;
    bool writable;
};
inline tAbstractStreamCommand* Create_tStreamSaveLoginWritable()
{
    return new tStreamSaveLoginWritable;
}
//****************************************************************
class tStreamReportSaveLoginWritable : public tAbstractStreamCommand
{
    Q_OBJECT
public:
    virtual bool Initialize(QDataStream &){return true;}
    virtual bool ExeCommand(QDataStream &);
    virtual void ProcessError(QDataStream &){}
    virtual void ExternalExit() {emit EndCommand();}

private:
    QByteArray block1;
};
inline tAbstractStreamCommand* Create_tStreamReportSaveLoginWritable()
{
    return new tStreamReportSaveLoginWritable;
}
#endif // TCLIENTSTREAMCOMMANDS_H
