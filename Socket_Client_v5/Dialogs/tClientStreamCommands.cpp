#include "tClientStreamCommands.h"
#include <QDir>
#include <QFileInfo>


extern tSettings my_settings;
//-----------------------------------------------------------------
bool tStreamReceiveFile::Initialize(QDataStream& _in)
{
    error_reason="Reason: ";
    error_detected=false;
    enabled=true;
    read_size=0;

    //    _in.device()->reset();
    _in >> file_name;
    _in >> file_size;


    _in >> hidden;
    _in >> create_date_time;
    _in >> last_modified;
    _in >> permission;
    _in >> hash;

    //    l="tStreamReceiveFile \tInitialize\t Инициализация приема файла "+file_name.toUtf8();
    //    log.Write(l);

    root=my_settings.GetTemp()+"Receive/";

    file.setFileName(root+file_name);

    QDir dir(root);

    QFileInfo info(root+file_name);

    QString path=info.absolutePath();

    dir.mkpath(path);

    SetFileAttributes((LPCWSTR)(root+file_name).data(), !FILE_ATTRIBUTE_HIDDEN);

    calc_hash=new tCalcHash();

    file.open(QIODevice::WriteOnly);

    if(!file.isOpen())
    {
        error_reason=error_reason+"Write file error ";
    }
    error_detected=!file.isOpen();
    return true;
}
//-----------------------------------------------------------------
bool tStreamReceiveFile::ExeCommand(QDataStream& _in)
{
    bool ret=false;



    qint64 n=_in.device()->bytesAvailable();
    char* buff= new char[n];
    qint64 num=_in.device()->read(buff, n);
    read_size=read_size+n;

    if(enabled)
    {
        if(!error_detected)
        {
            file.write(buff, num);
        }

        QByteArray data(buff, n);
        calc_hash->AddToHash(data);

        delete[] buff;
        if(read_size==file_size)
        {
            l="tStreamReceiveFile \tExeCommand\t Прием файла "+file_name.toUtf8()+" завершен";
            log.Write(l);

            ret=true;
            qDebug() << "File transfer complete";

            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_8);

            QString file_name=file.fileName();
            qint64 file_size=file.size();
            QString command="ReceiveReport";

            file.close();

            bool error_detected1=!SetAttributeFile(file_name, hidden, create_date_time, last_modified, permission, hash);

            if(!error_detected1 && !error_detected)
            {
                out << command;
                out << file_name;
                out << file_size;

                emit Result(block);
                emit EndCommand();
            }
            else
            {
                ProcessError(out);
            }
        }
    }
    else
    {
        delete[] buff;
        ret=true;
        if(read_size==file_size)
        {
            emit EndCommand();
        }
    }
    return ret;
}
//-----------------------------------------------------------------
void tStreamReceiveFile::ProcessError(QDataStream &)
{

    enabled=false;
    int num_error=1;
    QString error="Error in command 1";
    QString detail="File name: "+file_name+"\nFile size: "+QString::number(file_size);

    QString client_detail="Ошибка на стороне клиента \n"+error_reason;

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    l="tStreamReceiveFile \tProcessError\t Ошибка приема файла "+detail.toUtf8();
    log.Write(l);

    emit Result(block);
    emit EndCommand();

}
//-----------------------------------------------------------------
bool tStreamReceiveFile::SetAttributeFile(const QString &_file_name, const bool _hidden, const QDateTime &_create_date_time, const QDateTime &_last_modified, const int _permission, const QString &_hash)
{
    QString s=_file_name;

    if(_hidden)
    {

        SetFileAttributes((LPCWSTR)s.data(), FILE_ATTRIBUTE_HIDDEN);
    }
    else
    {
        SetFileAttributes((LPCWSTR)s.data(), !FILE_ATTRIBUTE_HIDDEN);
    }

    QFile::Permissions perm=_permission;
    file.setPermissions(perm);

    cr_dt=QDateTimeToFILETIME(_create_date_time);
    last_dt=QDateTimeToFILETIME(_last_modified);


    s.replace("/", "\\");
    LPCWSTR s1=(LPCWSTR)s.utf16();

    handle=CreateFile(s1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(_hash!=calc_hash->ResultHash())
    {
        error_reason=error_reason+"Hash error ";
    }
    if(file_size!=file.size())
    {
        error_reason=error_reason+"Size file error ";
    }

    SetFileTime(handle,&cr_dt, &last_dt, &last_dt);
    CloseHandle(handle);
    return _hash==calc_hash->ResultHash() && file_size==file.size();
}
//-----------------------------------------------------------------
FILETIME tStreamReceiveFile::QDateTimeToFILETIME(const QDateTime &_dt) const
{
    SYSTEMTIME system_time;
    FILETIME file_time, localFileTime;

    system_time.wYear = _dt.date().year();
    system_time.wMonth = _dt.date().month();
    system_time.wDayOfWeek = _dt.date().dayOfWeek();
    system_time.wDay = _dt.date().day();
    system_time.wHour = _dt.time().hour();
    system_time.wMinute = _dt.time().minute();
    system_time.wSecond = _dt.time().second();
    system_time.wMilliseconds = _dt.time().msec();
    SystemTimeToFileTime(&system_time, &localFileTime);
    LocalFileTimeToFileTime(&localFileTime, &file_time);

    return file_time;
}
//-----------------------------------------------------------------
tStreamReceiveFile::~tStreamReceiveFile()
{
    delete calc_hash;
}

//*****************************************************************
//-----------------------------------------------------------------
bool tPrepareReceiveFile::Initialize(QDataStream &_in)
{

    _in >> file_name;
    _in >> root;
//    root=my_settings.GetRoot();
    QString f=root+file_name;
    file.setFileName(root+file_name);
    file.open(QIODevice::Append);


    error_detected=file.exists() && !file.isOpen();

    //    l="tPrepareReceiveFile \tInitialize\t Инициализация подготовки приема файла "+f.toUtf8();
    //    log.Write(l);

    return true;
}
//-----------------------------------------------------------------
bool tPrepareReceiveFile::ExeCommand(QDataStream &_in)
{
    if(!error_detected)
    {
        l="tPrepareReceiveFile \tExeCommand\t Отправление команды приема файла на сервер ";
        log.Write(l);

        QString comm="Command:";
        int num_comm=1;

        _in << quint16(0);

        _in << comm;
        _in << num_comm;
        _in << file_name;

        _in.device()->seek(0);

        quint16 bs=(quint16)(_in.device()->size()) - sizeof(quint16);
        _in << bs;
    }
    else
    {
        ProcessError(_in);
    }
    return false;
}
//-----------------------------------------------------------------
void tPrepareReceiveFile::ProcessError(QDataStream & )
{
    int num_error=1;
    QString error="Error in command 1";
    QString detail="File name: "+file_name+"\nFile is open ";

    QString client_detail="Ошибка на стороне клиента \nWrite file error ";

    l="tPrepareReceiveFile \tProcessError\t Ошибка в подготовке приема файла "+detail.toUtf8();
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    emit Result(block);
    emit EndCommand();
}
//-----------------------------------------------------------------
void tPrepareReceiveFile::ExternalExit()
{
    emit EndCommand();
}

//*****************************************************************
//-----------------------------------------------------------------
bool tSendFile::Initialize(QDataStream &_in)
{
    root=my_settings.GetRoot();
    _in >> file_name;

    //    out << file_name;
    //    out << num_err;
    //    out << error;
    //    out << detail;
    //    out << client_detail;
    //    out << Hash;


    num_err=0;
    error="";
    detail="";
    client_detail="";
    Hash="";


    _in >> num_err;
    _in >> error;
    _in >> detail;
    _in >> client_detail;
    _in >> Hash;


    file.setFileName(root+file_name);
    file.open(QIODevice::ReadOnly);

    Continue=false;

    QString s=root+file_name;


    return file.isOpen();
}
//-----------------------------------------------------------------
bool tSendFile::ExeCommand(QDataStream &_out)
{
    bool ret=false;

    if(!Continue)
    {
        QString Comm="Command:";
        int num_comm=2;

        _out << quint16(0);

        _out << Comm;
        _out << num_comm;
        _out << file_name;
        _out << file.size();

        l="tSendFile \tExeCommand\t Отправка на сервер команды передачи файла "+file_name.toUtf8();
        log.Write(l);

        //остальные параметры передаваемого файла
        //скрытость, дата создания, дата модификации, права, хэш-сумма

        QFileInfo info(file_name);

        bool hidden=info.isHidden();
        QDateTime create_date_time=info.created();
        QDateTime last_modified=info.lastModified();
        int permission=info.permissions();
        //        tCalcHash ch;
        //        QString hash=ch.GetFileHash(root+file_name);

        _out << hidden;
        _out << create_date_time;
        _out << last_modified;
        _out << permission;
        _out << Hash;

        _out.device()->seek(0);
        quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
        _out << bs;
        Continue=true;
        _out.device()->seek(_out.device()->size());
    }
    else
    {

        _out.device()->seek(0);

        char* buff=new char[32768*8];


        qint64 n=file.read(buff, 32768*8);
        qDebug() << n;
        if(n==0)
        {
            file.close();
            ret=true;
            emit EndCommand();
        }
        _out.device()->write(buff,n);

        delete[] buff;
    }
    return ret;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tReportSendFile::ExeCommand(QDataStream &_out)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString file_name="";
    qint64 file_size=-1;
    _out >> file_name;
    _out >> file_size;
    QString command="SendReport";

    l="tReportSendFile \tExeCommand\t Отчет о передаче файла "+file_name.toUtf8();
    log.Write(l);

    out << command;
    out << file_name;
    out << file_size;

    emit Result(block);

    return true;
}
//-----------------------------------------------------------------
void tReportSendFile::ProcessError(QDataStream &_in)
{
    int num_error=2;
    QString error="";
    QString detail="";

    QString client_detail="Ошибка на стороне сервера. \nОтправка файла завершилась неудачей";

    _in >> error;
    _in >> detail;

    l="tReportSendFile \tProcessError\t Ошибка в передаче файла "+detail.toUtf8();
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
void tProcessingError::ProcessError(QDataStream &_in)
{
    int num_error=-1;
    QString error="";
    QString detail="";

    QString client_detail="Ошибка на стороне сервера. \nПередача команды на сервер завершилась неудачей";

    _in >> num_error;
    _in >> error;
    _in >> detail;

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    l="tProcessingError \tProcessError\t Ошибка на сервере "+detail.toUtf8();
    log.Write(l);

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tConnectConfirmReport::ExeCommand(QDataStream &_in)
{
    bool ok=false;

    _in >> ok;

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("ConnectConfirmReport");
    in << ok;

    l="tConnectConfirmReport \tExeCommand\t Подтверждение соединения ";
    log.Write(l);

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamPrepareSendFile::Initialize(QDataStream &_in)
{
    file_name="";

    _in >> file_name;

    //    l="tStreamPrepareSendFile \tInitialize\t Инициализация подготовки передачи файла "+file_name.toUtf8();
    //    log.Write(l);

    return true;
}
//-----------------------------------------------------------------
bool tStreamPrepareSendFile::ExeCommand(QDataStream &_out)
{
    root=my_settings.GetRoot();
    QString Comm="Command:";
    int num_comm=4;

    l="tStreamPrepareSendFile \tExeCommand\t Отправка на сервер команды подготовки к передаче файла  "+file_name.toUtf8();
    log.Write(l);

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;
    QString fn=file_name.right(file_name.length()-root.length());
    _out << fn;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportPrepareSendFile::Initialize(QDataStream &_in)
{
    _in >> file_name;
    bool ret=false;
    _in >> ret;

    return ret;
}
//-----------------------------------------------------------------
bool tStreamReportPrepareSendFile::ExeCommand(QDataStream &)
{
    QByteArray block;
    QDataStream in(&block, QIODevice::ReadWrite);

    in << tr("ReportPrepareSendFile");
    in << file_name;

    l="tStreamReportPrepareSendFile \tExeCommand\t Отчет о команде подготовки к передаче файла  "+file_name.toUtf8();
    log.Write(l);

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
void tStreamReportPrepareSendFile::ProcessError(QDataStream &_out)
{
    _out >> file_name;
    bool ret=false;
    _out >> ret;

    QString server_detail="";
    _out >> server_detail ;
    QString client_detail="Ошибка на стороне сервера. \nФайл "+file_name+" занят";
    QString error="Error in send file";
    int num_error=4;

    l="tStreamReportPrepareSendFile \tProcessError\t Ошибка подготовки к передаче файла  "+server_detail.toUtf8();
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << server_detail;
    in << client_detail;

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamSendAutorization::Initialize(QDataStream &_in)
{
    _in >> login;
    _in >> password;

    //также потом как-то поставить проверку на только латиницу
    return login.length()!=0 && password.length()!=0;
}
//-----------------------------------------------------------------
bool tStreamSendAutorization::ExeCommand(QDataStream &_out)
{

    QString Comm="Command:";
    int num_comm=5;

    l="tStreamSendAutorization \tExeCommand\t Отправка авторизации  "+login.toUtf8();
    log.Write(l);

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;
    _out << login;
    _out << password;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    return false;
}
//-----------------------------------------------------------------
void tStreamSendAutorization::ProcessError(QDataStream &_in)
{
    QString error="";
    _in >> login;
    _in >> password;
    _in >> error;
    int num_error=5;

    QString detail="Login: "+login+"\nPassword: "+password;

    QString client_detail="Ошибка на стороне клиента \nотсутствует логин или пароль ";

    l="tStreamSendAutorization \tProcessError\t Ошибка отправки авторизации "+client_detail.toUtf8();
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportAutorization::Initialize(QDataStream &_in)
{
    bool ret=false;
    _in >> ret;
    return ret;
}
//-----------------------------------------------------------------
bool tStreamReportAutorization::ExeCommand(QDataStream &)
{
    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    bool ret=true;
    //готовим команду для продолжения работы
    in << tr("ReportAutorization");
    in << ret;

    l="tStreamReportAutorization \tExeCommand\t Авторизация успешна ";
    log.Write(l);

    emit Result(block);

    return true;
}
//-----------------------------------------------------------------
void tStreamReportAutorization::ProcessError(QDataStream &)
{
    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    bool ret=false;
    //готовим команду для прекращения работы
    in << tr("ReportAutorization");
    in << ret;

    l="tStreamReportAutorization \tProcessError\t Авторизация неудачна ";
    log.Write(l);

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamStartTransaction::Initialize(QDataStream &_in)
{
    int num_files=-1;

    _in >> num_files;
    tFileList fl;
    file_list.clear();

    for(int i=0; i<num_files; i++)
    {
        _in >> fl.file_name;
        _in >> fl.client_hash;
        _in >> fl.server_hash;

        file_list << fl;
    }

    _in >> send;

    return true;
}
//-----------------------------------------------------------------
bool tStreamStartTransaction::ExeCommand(QDataStream &_out)
{

    l="tStreamStartTransactionn \tExeCommand\t Передача команды начала транзакции ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=6;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;
    _out << file_list.size();

    for(int i=0; i<file_list.size(); i++)
    {
        _out << file_list[i].file_name;
        _out << file_list[i].client_hash;
        _out << file_list[i].server_hash;
    }

    _out << send;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    file_list.clear();
    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportStartTransaction::Initialize(QDataStream &_in)
{
    bool ret=false;

    _in >> ret;

    return ret;
}
//-----------------------------------------------------------------
bool tStreamReportStartTransaction::ExeCommand(QDataStream &)
{
    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("ReportStartTransaction");

    l="tStreamReportStartTransaction \tExeCommand\t Транзакция начата успешно ";
    log.Write(l);

    emit Result(block);

    return true;
}

//-----------------------------------------------------------------
void tStreamReportStartTransaction::ProcessError(QDataStream &_in)
{
    //Коллизия при записи файлов
    QString error="";
    _in >> error;
    int num_error=6;

    QString detail="Коллизия в транзакции";

    QString client_detail="Ошибка на стороне сервера \nнеобходимо заново прочитать список файлов \nи повторно отправить файлы ";

    l="tStreamReportStartTransaction \tProcessError\t Коллизия в транзакции "+client_detail;
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamCommitTransaction::Initialize(QDataStream &_in)
{
    send=-1;
    _in >> send;

    //    model_file="";
    //    _in >> model_file;

    int num_files=-1;
    _in >> num_files;

    for(int i=0; i<num_files; i++)
    {
        QString file_name="";
        _in >> file_name;
        QString loc_hash="";
        _in >> loc_hash;

        QString serv_hash="";
        _in >> serv_hash;

        tFileList fl;
        fl.file_name=file_name;


        fl.client_hash=loc_hash;
        fl.server_hash=serv_hash;

        file_list.push_back(fl);
    }

    return true;
}
//-----------------------------------------------------------------
bool tStreamCommitTransaction::ExeCommand(QDataStream &_out)
{
    QString Comm="Command:";
    int num_comm=7;

    l="tStreamCommitTransaction \tExeCommand\t Отправка команды коммита транзакции ";
    log.Write(l);

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;


    _out << send;

    //    _out << model_file;

    int num_files=file_list.size();

    _out << num_files;

    for(int i=0; i<num_files; i++)
    {
        _out << file_list[i].file_name;
        //        if(send)
        //        {
        //            //отправка файла
        //            _out << file_list[i].client_hash;
        //        }
        //        else
        //        {
        //            //прием файла
        //            _out << file_list[i].server_hash;
        //        }
        _out << file_list[i].client_hash;
        _out << file_list[i].server_hash;
    }

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportCommitTransaction::Initialize(QDataStream &_in)
{
    bool ret=false;

    _in >> ret;

    return ret;
}
//-----------------------------------------------------------------
bool tStreamReportCommitTransaction::ExeCommand(QDataStream &)
{
    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("ReportCommitTransaction");

    l="tStreamReportCommitTransaction \tExeCommand\t Коммит завершился успешно ";
    log.Write(l);

    emit Result(block);

    return true;
}

//-----------------------------------------------------------------
void tStreamReportCommitTransaction::ProcessError(QDataStream &_in)
{
    //должно сообщать с каким файлом произошла проблема
    QString error_file="";
    _in >> error_file;

    QString error=QString::fromUtf8("Ошибка копирования файла во время завершения транзакции");

    QString detail=QString::fromUtf8("Ошибочный объект: ")+error_file;

    QString client_detail=QString::fromUtf8("Ошибка на стороне сервера \nТранзакция завершилась неудачей ");
    int num_error=7;

    l="tStreamReportCommitTransaction \tProcessError\t Ошибка коммита транзакции "+detail;
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("Error");
    in << num_error;
    in << error;
    in << detail;
    in << client_detail;

    emit Result(block);
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamCancelTransaction::ExeCommand(QDataStream &_out)
{
    l="tStreamCancelTransaction \tExeCommand\t Отмена транзакции ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=8;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    return false;
}
//-----------------------------------------------------------------
void tStreamCancelTransaction::ProcessError(QDataStream &)
{
    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("ReportCancelTransaction");

    l="tStreamCancelTransaction \tProcessError\t Ошибка отмены транзакции ";
    log.Write(l);

    emit Result(block);

    //    return true;
}

//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportCancelTransaction::ExeCommand(QDataStream &)
{
    l="tStreamReportCancelTransaction \tExeCommand\t Отчет об отмене транзакции ";
    log.Write(l);

    QByteArray block;
    QDataStream in(&block, QIODevice::WriteOnly);

    in << tr("ReportCancelTransaction");

    emit Result(block);

    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tDeleteFile::Initialize(QDataStream &_in)
{
    _in >> file_name;
    _in >> hash;
    return true;
}
//-----------------------------------------------------------------
bool tDeleteFile::ExeCommand(QDataStream &_out)
{
    QString Comm="Command:";
    int num_comm=9;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;
    _out << file_name;
    _out << hash;

    l="tDeleteFile \tExeCommand\t Отправка команды удаления файла с сервера "+file_name.toUtf8();
    log.Write(l);

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());

    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tReportDeleteFile::ExeCommand(QDataStream &_out)
{


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);

    QString file_name="";
    _out >> file_name;
    QString command="DeleteReport";

    out << command;
    out << file_name;

    l="tReportDeleteFile \tExeCommand\t Отчет об удалении файла с сервера "+file_name.toUtf8();
    log.Write(l);

    emit Result(block);

    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tGetListFiles::ExeCommand(QDataStream &_out)
{
    l="tGetListFiles \tExeCommand\t Запрос списка файлов с сервера ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=10;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());

    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tReportGetListFiles::Initialize(QDataStream &_in)
{
    num_files=-1;
    _in >> num_files;



    return true;
}
//-----------------------------------------------------------------
bool tReportGetListFiles::ExeCommand(QDataStream &_in)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("ReportGetListFiles");
    out << num_files;

    l="tReportGetListFiles \tExeCommand\t Список файлов в "+QString::number(num_files)+" шт";
    log.Write(l);

    for(int i=0; i<num_files; i++)
    {
        QString file="";
        _in >> file;
        out << file;
    }

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tGetServerListModels::ExeCommand(QDataStream &_out)
{
    l="tGetServerListModels \tExeCommand\t Запрос списка моделей с сервера ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=11;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());

    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tReportGetListServerModels::Initialize(QDataStream &_in)
{

    block1=_in.device()->readAll();
    return true;
}
//-----------------------------------------------------------------
bool tReportGetListServerModels::ExeCommand(QDataStream &)
{
    l="tReportGetListServerModels \tExeCommand\t Получение списка моделей с сервера ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("ReportGetListServerModels");
    out << block1;

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamSaveLoginPassword::Initialize(QDataStream &_in)
{
    login="";
    password="";
    new_user=false;
    num_log=0;
    row=-1;

    _in >> login;
    _in >> password;
    _in >> new_user;
    _in >> num_log;
    _in >> row;

    return true;
}
//-----------------------------------------------------------------
bool tStreamSaveLoginPassword::ExeCommand(QDataStream &_out)
{
    l="tSaveLoginPassword \tExeCommand\t Отправка на сервер данных о пользователе для регистрации ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=12;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out << login;
    _out << password;
    _out << new_user;
    _out << num_log;
    _out << row;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());

    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportSaveLoginPassword::Initialize(QDataStream &_in)
{
    s_num=0;
    _in >> s_num;
    return true;
}
//-----------------------------------------------------------------
bool tStreamReportSaveLoginPassword::ExeCommand(QDataStream &)
{
    l="tReportGetListServerModels \tExeCommand\t Регистрация данных пользователя прошла успешно ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReportSaveLoginPassword");
    out << s_num;

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
void tStreamReportSaveLoginPassword::ProcessError(QDataStream &_in)
{
    QString mess="";
    _in >> mess;

    int num_error=12;
    QString error="Error in command 12";
    QString detail=mess;

    QString client_detail="Логин не удалось записать ";

    l="tStreamReportSaveLoginPassword \tProcessError\t "+mess;
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("Error");
    out << num_error;
    out << error;
    out << detail;
    out << client_detail;

    emit Result(block);
    emit EndCommand();
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamDeleteLogin::Initialize(QDataStream &_in)
{
    num_login=0;
    _in >> num_login;
    return true;
}
//-----------------------------------------------------------------
bool tStreamDeleteLogin::ExeCommand(QDataStream &_out)
{
    l="tStreamDeleteLogin \tExeCommand\t Команда удаления пользователя ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=13;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out << num_login;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());
    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportDeleteLogin::Initialize(QDataStream &_in)
{
    s_num=0;
    _in >> s_num;
    return true;
}
//-----------------------------------------------------------------
bool tStreamReportDeleteLogin::ExeCommand(QDataStream &)
{
    l="tStreamReportDeleteLogin \tExeCommand\t Удаление пользователя прошло успешно ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReportDeleteLogin");
    out << s_num;

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
void tStreamReportDeleteLogin::ProcessError(QDataStream &_in)
{
    QString mess="";
    _in >> mess;

    int num_error=12;
    QString error="Error in command 13";
    QString detail=mess;

    QString client_detail="Пользователя не удалось удалить ";

    l="tStreamReportDeleteLogin \tProcessError\t "+mess;
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("Error");
    out << num_error;
    out << error;
    out << detail;
    out << client_detail;

    emit Result(block);
    emit EndCommand();
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tGetLoginsTable::ExeCommand(QDataStream &_out)
{
    l="tGetLoginsTable \tExeCommand\t Команда запроса таблицы логинов ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=14;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());
    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportReceiveLoginsTable::Initialize(QDataStream &_in)
{

    block1=_in.device()->readAll();
    return true;
}
//-----------------------------------------------------------------
bool tStreamReportReceiveLoginsTable::ExeCommand(QDataStream &)
{
    l="tStreamReportReceiveLoginsTable \tExeCommand\t Получение списка логинов с сервера ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReportReceiveLoginsTable");
    out << block1;

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReceiveReadPermissions::ExeCommand(QDataStream &_out)
{
    l="tStreamReceiveReadPermissions \tExeCommand\t Команда запроса таблицы разрешений на чтение ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=15;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());
    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportReceiveReadPermissions::Initialize(QDataStream &_in)
{

    block1=_in.device()->readAll();
    return true;
}
//-----------------------------------------------------------------
bool tStreamReportReceiveReadPermissions::ExeCommand(QDataStream &)
{
    l="tStreamReportReceiveLoginsTable \tExeCommand\t Получение списка разрешений на чтение моделей с сервера ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReportReceiveReadPermissions");
    out << block1;

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamSavePermissions::Initialize(QDataStream &_in)
{
//    _in >> block1;
    block1=_in.device()->readAll();
    return true;
}
//-----------------------------------------------------------------
bool tStreamSavePermissions::ExeCommand(QDataStream &_out)
{
    l="tStreamReceiveReadPermissions \tExeCommand\t Команда записи таблицы разрешений на сервер с таблицей ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=16;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;
    _out << block1;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());
    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportSavePermissions::ExeCommand(QDataStream &)
{
    l="tStreamReportReceiveLoginsTable \tExeCommand\t Получение ответа на запись разрешений на сервре ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReportSavePermissions");

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------

bool tStreamSaveLoginWritable::Initialize(QDataStream &_in)
{
    _in >> row;
    _in >> writable;
    return true;
}
//-----------------------------------------------------------------
bool tStreamSaveLoginWritable::ExeCommand(QDataStream &_out)
{
    l="tStreamSaveLoginWritable \tExeCommand\t Команда записи разрешений на запись на сервер с таблицей ";
    log.Write(l);

    QString Comm="Command:";
    int num_comm=17;

    _out << quint16(0);

    _out << Comm;
    _out << num_comm;
    _out << row;
    _out << writable;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
    _out.device()->seek(_out.device()->size());
    return false;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
bool tStreamReportSaveLoginWritable::ExeCommand(QDataStream &)
{
    l="tStreamReportSaveLoginWritable \tExeCommand\t Получение ответа на запись разрешений на запись логина на сервре ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GUIReportSaveLoginWritable");

    emit Result(block);
    return true;
}
//-----------------------------------------------------------------
//*****************************************************************
//-----------------------------------------------------------------
