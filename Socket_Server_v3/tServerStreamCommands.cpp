#include "tServerStreamCommands.h"
#include "Client.h"
#include "tFileBlocker.h"


extern tSettings my_settings;
extern tFileBlocker blocker;
//**********************************************************
//----------------------------------------------------------
bool tSendStreamFile::Initialize(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;
    _in.device()->reset();
    _in >> file_name;
    root=my_settings.GetRoot();
    file.setFileName(root+file_name);
    file.open(QIODevice::ReadOnly);

    qDebug() << file_name << file.size();
    cont=false;
    return file.isOpen() && ((tClient*)link)->IsTransaction();
}
//----------------------------------------------------------
bool tSendStreamFile::ExeCommand(QDataStream&, QDataStream &_out)
{


    bool ret=false;
    if(!cont)
    {

        log.Write(QString(QString::fromUtf8("tSendStreamFile \t ExeCommand \t ****** ОТПРАВКА ФАЙЛА ****** ")+file_name));

        cont=true;
        QString comm="Report:";
        int num_comm=1;

        qint64 file_size=file.size();
        qDebug() << file_size;
        _out << quint16(0);

        _out << comm;
        _out << num_comm;
        _out << file_name;
        _out << file_size;

        //остальные параметры передаваемого файла
        //скрытость, дата создания, дата модификации, права, хэш-сумма

        QFileInfo info(root+file_name);

        bool hidden=info.isHidden();
        QDateTime create_date_time=info.created();
        QDateTime last_modified=info.lastModified();
        int permission=info.permissions();
        //        QString hash=GetFileHash(file_name);

        _out << hidden;
        _out << create_date_time;
        _out << last_modified;
        _out << permission;
        //Пока хэш-сумма считается, потом буду брать из базы а считать только в исключительном случае
        //несовпадения размера и/или даты модификации с той что в базе
        tCalcHash *h=new tCalcHash();
        QString hh=h->GetFileHash(root+file_name);
        _out << hh;
        delete h;
        h=NULL;

        _out.device()->seek(0);
        quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
        _out << bs;
    }
    else
    {
        //теперь изменим cont и будем сразу попадать на передачу очередной порции файла
        cont=true;
        _out.device()->seek(0);

        char* buff=new char[32768*8];


        qint64 n=file.read(buff, 32768*8);
        if(n==0)
        {
            file.close();
            ret=true;
            log.Write(QString(QString::fromUtf8("tSendStreamFile \t ExeCommand \t Отправка файла ")+file_name+QString::fromUtf8(" завершена")));

            emit EndCommand();
        }
        _out.device()->write(buff,n);

        delete[] buff;
    }
    return ret;
}
//----------------------------------------------------------
void tSendStreamFile::SendErrorReport(QDataStream &_in)
{

    log.Write(QString(QString::fromUtf8("tSendStreamFile \t SendErrorReport \t Ошибка при передаче файла ")+file_name));
    //если возникла ошибка при передаче то дальнейшее выполнение транзакции
    //будет прервано и надо отменить транзакцию
    QString login=((tClient*)link)->GetName();
    blocker.UnlockReadWrite(login);
    blocker.map_blocker->unlockReadWrite(login);

    _in.device()->reset();
    QString detail="Root: "+root+"\n File name: "+file_name+"\n Reason: can not open file";
    int num_error=1;
    QString error="";
    _in >> error;

    _in.device()->reset();
    _in.device()->seek(0);

    //Формирование отчета об ошибке
    QString err="Error:";
    _in << quint16(0);
    _in << err;
    _in << num_error;
    _in << error;
    _in << detail;

    _in.device()->seek(0);
    quint16 bs=(quint16)(_in.device()->size() - sizeof(quint16));
    _in << bs;
    qDebug() << "Err " << err << "NumError" << num_error << "Error" << error << "Detail" << detail;
    emit EndCommand();
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tReceiveStreamFile::Initialize(QDataStream& _in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;



    error_reason="Reason: ";
    error_detected=false;
    _in.device()->reset();
    _in >> file_name;
    _in >> file_size;

    log.Write(QString(QString::fromUtf8("tReceiveStreamFile \t Initialize \t ****** ПРИЕМ ФАЙЛА ****** ")+file_name));
    root=my_settings.GetTemp();
    login=((tClient*)link)->GetName();
    root=root+login+"/";

    qDebug() << "file name " << file_name << "file size " << file_size << "root " << root;



    QFileInfo info(root+file_name);

    QString ad=info.absoluteDir().absolutePath();

    QDir direct(ad);
    direct.mkpath(ad);

    file.setFileName(root+file_name);

    SetFileAttributes((LPCWSTR)(root+file_name).data(), !FILE_ATTRIBUTE_HIDDEN);


    _in >> hidden;
    _in >> create_date_time;
    _in >> last_modified;
    _in >> permission;
    _in >> hash;

    enabled=true;
    read_size=0;
//    error_detected=!file.isOpen();
    calc_hash=new tCalcHash();

    if(!file.open(QIODevice::WriteOnly | QFile::Append) && ((tClient*)link)->IsTransaction())
    {
        log.Write(QString(QString::fromUtf8("tReceiveStreamFile \t Initialize \t Файл ")+file_name+" невозможно открыть для записи"));
        error_reason=error_reason+"Write file error ";
        error_detected=true;

    }
    else
    {
        error_detected=false;

    }
    return !error_detected;
}
//----------------------------------------------------------
bool tReceiveStreamFile::ExeCommand(QDataStream& _in, QDataStream& _out)
{

    //прием файла, в первый раз упраление будет передано из цикла, управляемого
    //возвращаемым из ExeCommand значением
    //поэтому обработаем первую порцию данных если она будет и выйдем из цикла
    //(отправка true)
    //а последующие порции будут сразу приходить без цикла но там возвращаемое
    //значение не проверяется

    bool ret=true;


    qint64 n=_in.device()->bytesAvailable();
    char* buff= new char[n];
    qint64 num=_in.device()->read(buff, n);
    read_size=read_size+n;
    if(enabled)
    {
        if(!error_detected)
        {
            file.write(buff, num);

            QByteArray data(buff, n);
            calc_hash->AddToHash(data);

            delete[] buff;
            qDebug() << "File size " << file_size << " saved size " << file.size() << " now " << num;
            if(read_size==file_size)
            {
                log.Write(QString(QString::fromUtf8("tReceiveStreamFile \t ExeCommand \t Прием файла ")+file_name+QString::fromUtf8(" завершен")));
                ret=false;
                file.close();

                if(!error_detected)
                {
                    QString receive_hash=calc_hash->ResultHash();
                    if(receive_hash==hash)
                    {
                    error_detected=!SetAttributeFile(root, file_name, hidden, create_date_time, last_modified, permission, hash);
                    }
                    else
                    {
                        error_detected=true;
                    }
                    SendReport(_out);
                }

            }
        }
        else
        {
            log.Write(QString(QString::fromUtf8("tReceiveStreamFile \t ExeCommand \t ошибка при приеме файла ")+file_name));
            delete[] buff;
            ret=true;
            if(read_size==file_size)
            {

                ret=false;
            }
        }
    }
    else
    {
        delete[] buff;
        ret=true;
    }
    return ret;
}
//----------------------------------------------------------
void tReceiveStreamFile::ExternalExit()
{
if(read_size==file_size)
{

    emit EndCommand();
}
}
//----------------------------------------------------------
void tReceiveStreamFile::SendErrorReport(QDataStream &_in)
{
    log.Write(QString(QString::fromUtf8("tReceiveStreamFile \t SendErrorReport \t При приеме файла ")+file_name+" произошла ошибка"));

    //если возникла ошибка при приеме то дальнейшее выполнение транзакции
    //будет прервано и надо отменить транзакцию
    QString login=((tClient*)link)->GetName();
    blocker.UnlockReadWrite(login);
    blocker.map_blocker->unlockReadWrite(login);

    enabled=false;

    QString detail="Root: "+root+" File name: "+file_name+"\n"+error_reason;
    int num_error=2;
    QString error="Невозможно изменить файл";

    //Формирование отчета об ошибке
    QString err="Error:";
    _in << quint16(0);
    _in << err;
    _in << num_error;
    _in << error;
    _in << detail;
    _in.device()->seek(0);
    quint16 bs=(quint16)(_in.device()->size() - sizeof(quint16));
    _in << bs;
    qDebug() << "Err " << err << "NumError" << num_error << "Error" << error << "Detail" << detail;
    emit EndCommand();
}
//----------------------------------------------------------
void tReceiveStreamFile::SendReport(QDataStream &_out)
{
    if(!error_detected)
    {
        log.Write(QString(QString::fromUtf8("tReceiveStreamFile \t SendReport \t Посылка отчета о приеме файла ")+file_name));

        QString comm="Report:";
        int num_comm=2;

        QString fn=file.fileName();
        QString rp=root;
        QString fn1=fn.right(fn.length()-rp.length());
        qint64 file_size=file.size();
        qDebug() << file_size;
        _out << quint16(0);

        _out << comm;
        _out << num_comm;
        _out << fn1;
        _out << file_size;

        _out.device()->seek(0);
        quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
        _out << bs;

        emit EndCommand();
    }
    else
    {
        SendErrorReport(_out);
    }
}
//----------------------------------------------------------
FILETIME tReceiveStreamFile::QDateTimeToFILETIME(const QDateTime &_dt) const
{
    SYSTEMTIME system_time;
    FILETIME file_time, local_file_time;

    system_time.wYear = _dt.date().year();
    system_time.wMonth = _dt.date().month();
    system_time.wDayOfWeek = _dt.date().dayOfWeek();
    system_time.wDay = _dt.date().day();
    system_time.wHour = _dt.time().hour();
    system_time.wMinute = _dt.time().minute();
    system_time.wSecond = _dt.time().second();
    system_time.wMilliseconds = _dt.time().msec();
    SystemTimeToFileTime(&system_time, &local_file_time);
    LocalFileTimeToFileTime(&local_file_time, &file_time);

    return file_time;
}
//----------------------------------------------------------
bool tReceiveStreamFile::SetAttributeFile(const QString &_root, const QString &_file_name, const bool _hidden, const QDateTime _create_date_time, const QDateTime _last_modified, const int _permission, const QString &_hash)
{
    QString file_path=_root+_file_name;

    if(_hidden)
    {

        SetFileAttributes((LPCWSTR)file_path.data(), FILE_ATTRIBUTE_HIDDEN);
    }
    else
    {
        SetFileAttributes((LPCWSTR)file_path.data(), !FILE_ATTRIBUTE_HIDDEN);
    }

    QFile::Permissions perm=_permission;
    file.setPermissions(perm);

    cr_dt=QDateTimeToFILETIME(_create_date_time);
    last_dt=QDateTimeToFILETIME(_last_modified);


    file_path.replace("/", "\\");
    LPCWSTR s1=(LPCWSTR)file_path.utf16();

    handle=CreateFile(s1, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    SetFileTime(handle,&cr_dt, &last_dt, &last_dt);


    CloseHandle(handle);


    if(_hash!=calc_hash->ResultHash())
    {
        error_reason=error_reason+"Hash error ";
    }
    if(file_size!=file.size())
    {
        error_reason=error_reason+"Size file error ";
    }

    return _hash==calc_hash->ResultHash() && file_size==file.size();
}

//----------------------------------------------------------
tReceiveStreamFile::~tReceiveStreamFile()
{
    delete calc_hash;
    calc_hash=NULL;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
void tError::SendErrorReport(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    ((tClient*)link)->SetTransaction(false);

    //если возникла ошибка то дальнейшее выполнение транзакции
    //будет прервано и надо отменить транзакцию
    QString login=((tClient*)link)->GetName();
    blocker.UnlockReadWrite(login);
    blocker.map_blocker->unlockReadWrite(login);

    QString detail="Unspecified Error";
    int num_error=0;
    QString error="";
    _in >> error;

    _in.device()->reset();
    _in.device()->seek(0);

    //Формирование отчета об ошибке
    QString err="Error:0";
    _in << err;
    _in << num_error;
    _in << error;
    _in << detail;

    log.Write(QString(QString::fromUtf8("tError \t SendErrorReport \t Произошла ошибка ")+error));

    emit EndCommand();
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tConnectionReport::ExeCommand(QDataStream& _in, QDataStream&)
{
    tLog log1(QString("(Login: неопределен)"));
    log=log1;

    QString comm="Report:";
    int num_com=3;
    bool ok=true;

    _in << quint16(0);
    _in << comm;
    _in << num_com;
    _in << ok;
    _in.device()->seek(0);
    quint16 bs=(quint16)(_in.device()->size() - sizeof(quint16));
    _in << bs;

    if(ok)
    {
        log.Write(QString(QString::fromUtf8("tConnectionReport \t ExeCommand \t Отправка отчета о соединении (успех)")));
    }
    else
    {
        log.Write(QString(QString::fromUtf8("tConnectionReport \t ExeCommand \t Отправка отчета о соединении (неудача)")));
    }

    emit EndCommand();
    return true;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tPrepareReceiveFile::Initialize(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    root=my_settings.GetRoot();
    _in >> file_name;
    file.setFileName(root+file_name);

    if(((tClient*)link)->IsTransaction())
    {
    if(file.exists())
    {
        if(!file.open(QFile::WriteOnly | QFile::Append))
        {
            log.Write(QString(QString::fromUtf8("tPrepareReceiveFile \t Initialize \t Файл в процессе редактирования???")));
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        log.Write(QString(QString::fromUtf8("tPrepareReceiveFile \t Initialize \t Файл отсутствует на диске")));
        return true;
    }
    }
    else
    {
        log.Write(QString(QString::fromUtf8("tPrepareReceiveFile \t Initialize \t Не начата транзакция")));
        return true;
    }

}
//----------------------------------------------------------
bool tPrepareReceiveFile::ExeCommand(QDataStream &, QDataStream &_out)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    bool ret=true;
    QString comm="Report:";
    int num_com=4;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    _out << file_name;
    _out << ret;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    log.Write(QString(QString::fromUtf8(QString("tPrepareReceiveFile \t ExeCommand \t Отправка отчета о подготовке файла "+file_name+" к передаче").toAscii())));

    return false;
}
//----------------------------------------------------------
void tPrepareReceiveFile::SendErrorReport(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    //если возникла ошибка при приеме то дальнейшее выполнение транзакции
    //будет прервано и надо отменить транзакцию
    QString login=((tClient*)link)->GetName();
    blocker.UnlockReadWrite(login);
    blocker.map_blocker->unlockReadWrite(login);

    bool ret=false;
    QString comm="Error:";
    int num_com=4;

    _in.device()->reset();
    _in.device()->seek(0);


    _in << quint16(0);
    _in << comm;
    _in << num_com;

    _in << file_name;
    _in << ret;

    QString detail="File not open to write";

    _in << detail;

    _in.device()->seek(0);
    quint16 bs=(quint16)(_in.device()->size() - sizeof(quint16));
    _in << bs;

    log.Write(QString(QString::fromUtf8(QString("tPrepareReceiveFile \t SendErrorReport \t Ошибка при подготовке файла "+file_name+" к передаче").toAscii())));
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tAutorization::Initialize(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    login="";
    password="";

    _in >> login;
    _in >> password;

return true;
}
//----------------------------------------------------------
bool tAutorization::ExeCommand(QDataStream &, QDataStream &_out)
{

    //Тут будет подключение к базе данных, проверка логина и пароля но пока
    //буду пропускать если логин равен паролю
    //работа по подготовке временно папки этого пользователя если он авторизовался


    QString comm="Report:";
    int num_com=5;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    bool ret=login == password;
    _out << ret;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    if(ret)
    {

        ((tClient*)link)->SetName(login);
        log.Write(QString(QString::fromUtf8(QString("tAutorization \t ExeCommand \t Авторизация прошла успешно Login: "+login).toAscii())));
    }
    else
    {
        log.Write(QString(QString::fromUtf8(QString("tAutorization \t ExeCommand \t Авторизация не удалась Login: "+login+" Password: "+password).toAscii())));
    }

    return false;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tStartTransaction::Initialize(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

login=((tClient*)link)->GetName();
temp_path=my_settings.GetTemp();
root=my_settings.GetRoot();

int num_files=-1;

_in >> num_files;

for(int i=0; i<num_files; i++)
{
    tFileList fl;

    QString name="";

    _in >> name;
     fl.file_name=name;

    _in >> fl.client_hash;
    _in >> fl.server_hash;

    file_list << fl;
}

_in >> send_mode;


return VerifyCollisions();
}
//----------------------------------------------------------
bool tStartTransaction::ExeCommand(QDataStream &, QDataStream &_out)
{
    ((tClient*)link)->SetTransaction(true);

    if(send_mode)
    {
        log.Write(QString(QString::fromUtf8("tStartTransaction \t ExeCommand \t СТАРТ ТРАНЗАКЦИИ НА ЗАПИСЬ ")));
        qDebug() << QString::fromUtf8("Старт транзакции на запись");
        //***********************************************
        //***********************************************
        //НЕОБХОДИМО В ПЕРВУЮ ОЧЕРЕДЬ ДОБАВИТЬ В МЭП
        //ИДЕНТИФИКАТОР КЛИЕНТА ЧТО БЫ БЫЛО ИЗВЕСТНО
        //КАКОЙ КЛИЕНТ ВНЕС ЭТУ ЗАПИСЬ
        //***********************************************
        //***********************************************

        //БЛОКИРУЕМ СПИСОК НА ЗАПИСЬ И ДОБАВЛЯЕМ ФАЙЛЫ, ЗАКРЫТЫЕ НА ЗАПИСЬ

        //РАЗБЛОКИРОВКА ПРОИЗВОДИТСЯ В КОНЦЕ КОММИТА ТРАНЗАКЦИИ

        //    Blocker.map_blocker->lockWrite(login);

        //ЗАПИСЬ В МАП

        for(int i=0; i<file_list.size(); i++)
        {
            blocker.StartWriteFile(file_list[i].file_name, login);
        }
        file_list.clear();

        ClearTempFolder();
    }
    else
    {
        log.Write(QString(QString::fromUtf8("tStartTransaction \t ExeCommand \t СТАРТ ТРАНЗАКЦИИ НА ЧТЕНИЕ ")));
        qDebug() << QString::fromUtf8("Старт транзакции на чтение");
        for(int i=0; i<file_list.size(); i++)
        {
            blocker.StartReadFile(file_list[i].file_name, login);
        }
        file_list.clear();
    }

    bool ret=true;

    QString comm="Report:";
    int num_com=6;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    _out << ret;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    return false;
}
//----------------------------------------------------------
void tStartTransaction::ClearTempFolder()
{
    QString path=temp_path+login;//+"/";
    //удаление временной папки если она есть
    //делаем рекурсивный обход папок временной папки с удалением всех файлов
    //после чего удаляем всю папку
    dir.setPath(path);
    removeFolder(dir);
    //создание временной папки

    for(;dir.exists();)
    {
        removeFolder(dir);
    }

    for(;!dir.exists();)
    {
    dir.mkpath(dir.path());
    }
}
//----------------------------------------------------------
bool tStartTransaction::removeFolder(const QDir & _dir) const
{
  bool res = false;
  //Получаем список каталогов
  QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                  QDir::AllDirs |
                  QDir::NoDotAndDotDot | QDir::Hidden);
  //Получаем список файлов
  QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

  //Удаляем файлы
  foreach (QString entry, lst_files)
  {
   QString entry_abs_path = _dir.absolutePath() + "/" + entry;
   QFile::setPermissions(entry_abs_path, QFile::ReadOwner | QFile::WriteOwner);
   QFile::remove(entry_abs_path);
  }

  //Для папок делаем рекурсивный вызов
  foreach (QString entry, lst_dirs)
  {
   QString entry_abs_path = _dir.absolutePath() + "/" + entry;
   QDir dr(entry_abs_path);
   removeFolder(dr);
  }

  //Удаляем обрабатываемую папку
  SetFileAttributes((LPCWSTR)(_dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
    QFile::setPermissions(_dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
  if (!QDir().rmdir(_dir.absolutePath()))
  {
    res = true;
  }
  return res;
}
//----------------------------------------------------------
bool tStartTransaction::VerifyCollisions()
{
    blocker.map_blocker->lockRead(login);

    for(int i=0; i<file_list.size(); i++)
    {
        //ПОКА ПРОВЕРЯЮ ТОЛЬКО ПО ИМЕНАМ ФАЙЛОВ
        //ПРОВЕРКУ НА СОВПАДЕНИЕ ХЕШЕЙ БУДУ ДЕЛАТЬ ПОЗДНЕЕ

        QString file_name=file_list[i].file_name;
        if(blocker.IsExists(file_name))
        {
            //СОВПАДЕНИЕ НАЙДЕНО
            //ФАЙЛ ЗАКРЫТ НА ЗАПИСЬ
            //ЭТО КОЛЛИЗИЯ
            //НАЧАЛО ТРАНЗАКЦИИ ОКОНЧИЛОСЬ НЕУДАЧЕЙ

            //ТАКЖЕ НУЖНО БУДЕТ ПРОВЕРЯТЬ ХЕШ ФАЙЛА С СЕРВЕРА И СЕРВЕРНЫЙ ХЕШ ФАЙЛА ИЗ СПИСКА ФАЙЛОВ
            //ЕСЛИ ОНИ БУДУТ ОТЛИЧАТЬСЯ ТО НАЧАЛО КОММИТА ТАКЖЕ ПРЕРЫВАЕТСЯ
            //НУЖНО ПРОИЗВЕСТИ ПОВТОРНОЕ ЧТЕНИЕ СПИСКА ФАЙЛОВ С СЕРВЕРА
            //(ПОКА НЕ РЕАЛИЗОВАНО)

            blocker.map_blocker->unlockRead(login);

            return false;
        }
    }

    blocker.map_blocker->unlockRead(login);

    //КОЛЛИЗИИ НЕ ОБНАРУЖЕНЫ



    return true;
}
//----------------------------------------------------------
void tStartTransaction::SendErrorReport(QDataStream &_out)
{
    log.Write(QString(QString::fromUtf8("tStartTransaction \t SendErrorReport \t Ошибка при старте коммита КОЛЛИЗИЯ ")));

    //если возникла ошибка при начале транзакции то дальнейшее выполнение транзакции
    //будет прервано и надо отменить транзакцию
    QString login=((tClient*)link)->GetName();

    QString comm="Error:";
    int num_com=6;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    _out << "Логин: "+login;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tCommitTransaction::Initialize(QDataStream &_in)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    send_mode=-1;
root=my_settings.GetRoot();

_in >> send_mode;

//model_file="";
//_in >> model_file;

    int num_files=-1;

    _in >> num_files;

    file_list.clear();
    for(int i=0; i<num_files; i++)
    {
        QString file_name="";
        _in >> file_name;
        QString cl_hash="";
        _in >> cl_hash;

        QString serv_hash="";
        _in >> serv_hash;

        tFileList fl;
        fl.client_hash=cl_hash;
        fl.file_name=file_name;
        fl.server_hash=serv_hash;

        file_list.push_back(fl);
    }
return true;
}
//----------------------------------------------------------
bool tCommitTransaction::ExeCommand(QDataStream &, QDataStream &_out)
{
    log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t ****** КОММИТ ТРАНЗАКЦИИ ******. Замена и удаление целевых файлов ")));

    QString login=((tClient*)link)->GetName();
    InitDB(((tClient*)link)->GetDB());
    if(send_mode==1)
    {

        temp=my_settings.GetTemp()+login+"/";
        root=my_settings.GetRoot();
        QDir dir(temp);
        stopped=false;
        error_file="";
        //Необходимо пройти по списку файлов и проверить возможность его перезаписи
        //перед началом копирования файлов
        //если скопировать нельзя то отменять транзакцию.

        //проверить хеш-суммы заменяемых файлов на совпадение с присланными серверными хеш-суммами
        //если не совпадает - прерываем транзакцию
        //проверка по списку file_list
        VerifyReplacedFiles();

        //добавить процедуру проверки возможности дозаписи (удаления) файлов, подлежащих удалению
        //проверяя их хеш-суммы и сверяя с присланными серверными
        //если что-то не так - прерываем транзакцию
        //проверка по списку file_list
        VerifyDeletedFiles();

        if(!stopped)
        {
            if(FolderOperation(dir, 0))
            {
                if(FolderOperation(dir, 1))
                {
                    if(((tClient*)link)->GetSizeDelFileList()!=0)
                    {
                        bool ok=true;
                        for(int i=0; i<((tClient*)link)->GetSizeDelFileList();i++)
                        {
                            QString file=((tClient*)link)->GetDelFile(i);
                            ok=Delete(root+file, error_file);
                            if(ok)
                            {
                                DeleteEmptyFolders(root);
                            }
                            else
                            {
                                SendError(_out);
                            }
                        }
                        if(ok)
                        {
                            SendOk(_out);
                        }
                        else
                        {
                            log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Ошибка при удалении файла ")));
                            SendError(_out);
                        }
                    }
                    else
                    {
                        SendOk(_out);
                    }
                }
                else
                {
                    log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Ошибка при перемещении файла ")));
                    SendError(_out);
                }
            }
            else
            {
                log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Ошибка при проверке файла ")));
                SendError(_out);
            }
        }
        else
        {
            log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Неожидданое изменение целевого файла ")));
            SendError(_out);
        }

        db_op->RefreshModelsFiles();

        log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Разблокировка от записи файла ")));

        for(int i=0; i<file_list.size(); i++)
        {
            //                QString NN=file_list[i];
            blocker.FinishWriteFile(file_list[i].file_name, login);
        }
        file_list.clear();
        ((tClient*)link)->ClearDelList();
    }
    else
    {
        log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Разблокировка от чтения файла ")));

        for(int i=0; i<file_list.size(); i++)
        {
            blocker.FinishReadFile(file_list[i].file_name, login);
        }
        file_list.clear();
        SendOk(_out);

    }
    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    ((tClient*)link)->SetTransaction(false);

    log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t ЗАВЕРШЕНИЕ ТРАНЗАКЦИИ ")));
    return false;
}
//----------------------------------------------------------
void  tCommitTransaction::VerifyReplacedFiles()
{
    //если все нормально - stopped=false
    //если ошибка - stopped=true
    for(int i=0; i<file_list.size(); i++)
    {
        QDateTime db_last_mod=db_op->GetLastMod(file_list[i].file_name);

        QFileInfo info(root+file_list[i].file_name);
        QDateTime disk_last_mod=db_op->RoundDateTime(info.lastModified());

        QString hash="";
        if(!db_last_mod.isNull() && info.exists())
        {
        if(db_last_mod==disk_last_mod)
        {
            //Дата-время совпадают
            //можно воспользоваться хешем из базы
            hash=db_op->GetHash(file_list[i].file_name);
        }
        else
        {
            //Дата-время не совпадают
            //пересчитать хеш и обновить дату-время модификации и хеш с базе
            tCalcHash ch;
            hash=ch.GetFileHash(root+file_list[i].file_name);

            db_op->Update_LastMod_Hash(file_list[i].file_name, disk_last_mod, hash);
        }

        //сравнить местный и присланый серверный или клиентский хеши
        //если не совпадают - ошибка
        stopped=(file_list[i].server_hash!=hash && file_list[i].client_hash!=hash);
        if(stopped)
        {
            log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Заменяемый файл неожиданно изменен ")));
            error_file=file_list[i].file_name;
        }
        }
    }
}
//----------------------------------------------------------
void tCommitTransaction::VerifyDeletedFiles()
{
    //если все нормально - stopped=false
    //если ошибка - stopped=true

for(int i=0; i<((tClient*)link)->GetSizeDelFileList(); i++)
{
    QDateTime db_last_mod=db_op->GetLastMod(((tClient*)link)->GetDelFile(i));

    QFileInfo info(root+((tClient*)link)->GetDelFile(i));
    QDateTime disk_last_mod=db_op->RoundDateTime(info.lastModified());

    QString hash="";
    if(!db_last_mod.isNull() && info.exists())
    {
    if(db_last_mod==disk_last_mod)
    {
        //Дата-время совпадают
        //можно воспользоваться хешем из базы
        hash=db_op->GetHash(((tClient*)link)->GetDelFile(i));
    }
    else
    {
        //Дата-время не совпадают
        //пересчитать хеш и обновить дату-время модификации и хеш с базе
        tCalcHash ch;
        QString hash=ch.GetFileHash(root+((tClient*)link)->GetDelFile(i));

        db_op->Update_LastMod_Hash(((tClient*)link)->GetDelFile(i), disk_last_mod, hash);
    }

    //сравнить местный и присланый хеши
    //если не совпадают - ошибка
    QString h=((tClient*)link)->GetDelHash(i);
    stopped=((tClient*)link)->GetDelHash(i)!=hash;
    if(stopped)
    {
        log.Write(QString(QString::fromUtf8("tCommitTransaction \t ExeCommand \t Удаляемый файл неожиданно изменен ")));
        error_file=((tClient*)link)->GetDelFile(i);
    }
    }
}
}
//----------------------------------------------------------
bool tCommitTransaction::FolderOperation(const QDir & _dir, const int _mode)
{
    //Получаем список каталогов
    QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

    //проверяем и перемещаем файлы
    foreach (QString entry, lst_files)
    {
        if(stopped) return false;

        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QString relat_path=entry_abs_path.right(entry_abs_path.length()-temp.length());
        QString new_abs_path=root+relat_path;

        QFile::setPermissions(new_abs_path, QFile::ReadOwner | QFile::WriteOwner);
        switch (_mode)
        {
            case 0:
            {
            Verify(new_abs_path, stopped, error_file);
                break;
            }
            case 1:
            {
            Move(entry_abs_path, new_abs_path, stopped, error_file);
                break;
            }
        }
    }
    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lst_dirs)
    {
        if(stopped) return false;

        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QString new_abs_path=root+entry_abs_path.right(entry_abs_path.length()-temp.length()); //root+entry;
        QDir dir(entry_abs_path);
        if(dir.mkpath(new_abs_path))
        {
            QDir dr(entry_abs_path);
            stopped=!FolderOperation(dr, _mode);
        }
        else
        {
            error_file=new_abs_path;
            stopped=true;
            return false;
        }
    }
      //Удаляем обрабатываемую папку
    if(_mode==1)
    {
      SetFileAttributes((LPCWSTR)(_dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
        QFile::setPermissions(_dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
      if (!QDir().rmdir(_dir.absolutePath()))
      {
          stopped=true;
        return false;
      }
    }


      return !stopped;
}
//----------------------------------------------------------
void tCommitTransaction::Move(const QString &_entry_abs_path, const QString &_new_abs_path, bool &_stopped, QString &_error_file)
{
    log.Write(QString(QString::fromUtf8("tCommitTransaction \t Move \t Замена файла ")+_new_abs_path));
    //В процессе перемещения файлов обновлять данные в базе данных (или добавлять новые записи если такой еще нет)
    QFile file_real(_new_abs_path);
    if(!file_real.exists() || file_real.remove())
    {
        QFile file_temp(_entry_abs_path);
        if(file_temp.copy(_new_abs_path))
        {
            //копирование файла удалось

            QFile::setPermissions(_new_abs_path, QFile::ReadOwner | QFile::WriteOwner);
//            db_op->UpdateFileInfo(_new_abs_path);


            if(!file_temp.remove())
            {
                log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Move \t Временный файл "+_entry_abs_path+" не может быть удален ").toAscii())));
                _error_file=file_temp.fileName();
                _stopped=true;
            }
        }
        else
        {
            log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Move \t Временный файл "+_entry_abs_path+" не может быть скопирован ").toAscii())));
            _error_file=file_temp.fileName();
            _stopped=true;
        }
    }
    else
    {
        log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Move \t Файл "+_new_abs_path+" отсутствует или не может быть удален ").toAscii())));
        _error_file=file_real.fileName();
        _stopped=true;
    }
}
//----------------------------------------------------------
void tCommitTransaction::Verify(const QString &_new_abs_path, bool &_stopped, QString &_error_file)
{
    log.Write(QString(QString::fromUtf8("tCommitTransaction \t Move \t Проверка файла ")+_new_abs_path));
    QFile file_real(_new_abs_path);
    if(file_real.exists())
    {
        if(file_real.open(QIODevice::Append))
        {

            _stopped=false;
            _error_file="";
        }
        else
        {
            log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Verify \t Файл "+_new_abs_path+" не может быть перезаписан ").toAscii())));
            _stopped=true;
            _error_file=file_real.fileName();
        }
    }
    else
    {
        log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Verify \t Файл "+_new_abs_path+" отсутствует ").toAscii())));
        _stopped=false;
        _error_file="";
    }
}
//----------------------------------------------------------
bool tCommitTransaction::Delete(const QString &_new_abs_path, QString &_error_file)
{
    log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Move \t Удаление файла "+_new_abs_path).toAscii())));
    //удаляем информацию о файле из базы
//    db_op->DeleteingFile(_new_abs_path);
    bool stopped=false;
    QFile file_real(_new_abs_path);
    if(file_real.exists())
    {
    stopped=!file_real.remove();
    }
    else
    {
        stopped=false;
    }
    if(stopped)
    {
        log.Write(QString(QString::fromUtf8(QString("tCommitTransaction \t Verify \t Файл "+_new_abs_path+" не может быть удален ").toAscii())));
        _error_file=file_real.fileName();
    }
    return !stopped;
}

//----------------------------------------------------------
void tCommitTransaction::SendError(QDataStream &_out)
{
    QString comm="Error:";
    int num_com=7;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    _out << error_file;
}
//----------------------------------------------------------
void tCommitTransaction::SendOk(QDataStream &_out)
{
    QString comm="Report:";
    int num_com=7;

    _out << quint16(0);
    _out << comm;
    _out << num_com;
    _out << true;
}

//----------------------------------------------------------
bool tCommitTransaction::DeleteEmptyFolders(const QString &_root)
{
//    bool res = false;
    QDir dir(_root);
    //Получаем список каталогов
    QStringList lst_dirs = dir.entryList(QDir::Dirs |
                                          QDir::AllDirs |
                                          QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = dir.entryList(QDir::Files | QDir::Hidden);

    if(lst_dirs.size()==0 && lst_files.size()==0)
    {
        //Если в папке нет ни файлов ни других папок то удаляем папку
        SetFileAttributes((LPCWSTR)(dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
        QFile::setPermissions(dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
        return QDir().rmdir(dir.absolutePath());

    }
    else
    {
        //Для папок делаем рекурсивный вызов
        foreach (QString entry, lst_dirs)
        {
            QString entry_abs_path = dir.absolutePath() + "/" + entry;
            DeleteEmptyFolders(entry_abs_path);

            //Если текущая папка стала пустой, удаляем ее
            QDir dir(_root);

            QStringList lst_dirs = dir.entryList(QDir::Dirs |
                                                  QDir::AllDirs |
                                                  QDir::NoDotAndDotDot | QDir::Hidden);
            //Получаем список файлов
            QStringList lst_files = dir.entryList(QDir::Files | QDir::Hidden);
            if(lst_dirs.size()==0 && lst_files.size()==0)
            {
                SetFileAttributes((LPCWSTR)(dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
                QFile::setPermissions(dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
                return QDir().rmdir(dir.absolutePath());
            }
        }
        return true;
    }
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tCancelTransaction::ExeCommand(QDataStream &, QDataStream &_out)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    log.Write(QString(QString::fromUtf8("tCancelTransaction \t ExeCommand \t ОТМЕНА ТРАНЗАКЦИИ ")));
    ((tClient*)link)->SetTransaction(false);

    //Отмена транзакции

    temp=my_settings.GetTemp()+((tClient*)link)->GetName()+"/";
    login=((tClient*)link)->GetName();
    blocker.UnlockReadWrite(login);
    blocker.map_blocker->unlockReadWrite(login);

//    //Очистка временной папки
//    ClearTempFolder();
//    ((tClient*)link)->ClearDelList();

    QString comm="Report:";
    int num_com=8;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    log.Write(QString(QString::fromUtf8("tCancelTransaction \t ExeCommand \t Отмена транзакции ")));

  return false;
}
//----------------------------------------------------------
void tCancelTransaction::ClearTempFolder()
{
    //удаление временной папки если она есть
    //делаем рекурсивный обход папок временной папки с удалением всех файлов
    //после чего удаляем всю папку
    dir.setPath(temp);
    removeFolder(dir);
    //создание временной папки

    for(;dir.exists();)
    {
        removeFolder(dir);
    }
}
//----------------------------------------------------------
bool tCancelTransaction::removeFolder(const QDir & _dir)
{
    bool res = false;
    //Получаем список каталогов
    QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

    //Удаляем файлы
    foreach (QString entry, lst_files)
    {
        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QFile::setPermissions(entry_abs_path, QFile::ReadOwner | QFile::WriteOwner);
        QFile::remove(entry_abs_path);
    }

    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lst_dirs)
    {
        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QDir dr(entry_abs_path);
        removeFolder(dr);
    }

    //Удаляем обрабатываемую папку
    SetFileAttributes((LPCWSTR)(_dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
    QFile::setPermissions(_dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
    if (!QDir().rmdir(_dir.absolutePath()))
    {
        res = true;
    }
    return res;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tDeleteFile::ExeCommand(QDataStream &_in, QDataStream &_out)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    QString del_file="";

    _in >> del_file;

    QString hash="";

    _in >> hash;

    ((tClient*)link)->AddDelList(del_file, hash);

    QString comm="Report:";
    int num_com=9;

    _out << quint16(0);
    _out << comm;
    _out << num_com;
    _out << del_file;

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    log.Write(QString(QString::fromUtf8(QString("tDeleteFile \t ExeCommand \t Отчет об удалении файла "+del_file).toAscii())));

    return false;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tGetFileList::ExeCommand(QDataStream &, QDataStream &_out)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    QString root=my_settings.GetRoot();
    QStringList list;
    CreateFileList(root, list);

    QString comm="Report:";
    int num_com=10;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    _out << list.size();

    for(int i=0; i<list.size(); i++)
    {
        QString f=list[i].right(list[i].length()-root.length());
        _out << list[i].right(list[i].length()-root.length());
    }

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    log.Write(QString(QString::fromUtf8("tGetFileList \t ExeCommand \t Передача списка файлов ")));

    return false;
}
//----------------------------------------------------------
void tGetFileList::CreateFileList(const QString &start_folder, QStringList& list)
{
    QDir dir(start_folder);
    SearchFiles(dir, list);
}
//----------------------------------------------------------
void tGetFileList::SearchFiles(const QDir & _dir, QStringList &_list)
{
    QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

    //записываем файлы
    foreach (QString entry, lst_files)
    {
        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        _list.push_back(entry_abs_path);
    }

    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lst_dirs)
    {
        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QDir dr(entry_abs_path);
        SearchFiles(dr, _list);
    }
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tGetListModels::ExeCommand(QDataStream &, QDataStream &_out)
{
    tLog log1(QString("(Login: "+((tClient*)link)->GetName()+")"));
    log=log1;

    //Передача всех данных о моделях и файлах клиенту во временную таблицу
    //для анализа списка обновляемых файлов

    //Необходимо сформировать и передать структуру данных следующего вида:
    //- Число моделей

    //----- Цикл по моделям
    //{
    //- DiskFile
    //- Title
    //- Description
    //- Struct
    //- LastMod
    //- Hash
    //- ListFilesLastMod
    //- ListFilesHash
    //- SummListHash
    //- Серверный номер модели
    //- Количество файлов модели

    //----- Цикл по файлам
    //{
    //- File
    //- Size
    //- LastMod
    //- Hash
    //- Серверный номер файла
    //}

    //}

    //Передачу данных между модулем работающим с БД и этим классом организуем через поток, в котором будут сериализованы все данные

    InitDB(((tClient*)link)->GetDB());

    QString comm="Report:";
    int num_com=11;

    _out << quint16(0);
    _out << comm;
    _out << num_com;

    db_op->GetListModels(_out);

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    log.Write(QString(QString::fromUtf8("tGetListModels \t ExeCommand \t Передача списка моделей ")));

    return false;
}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
bool tSaveLoginPassword::Initialize(QDataStream &_in)
{
    login="";
    password="";
    new_user=false;
    num_log=0;

    _in >> login;
    _in >> password;
    _in >> new_user;
    _in >> num_log;
    _in >> row;

    return true;
}
//----------------------------------------------------------
bool tSaveLoginPassword::ExeCommand(QDataStream &, QDataStream &_out)
{

//    qlonglong s_num=0;

    InitDB(((tClient*)link)->GetDB());
    QString mess=db_op->SaveLoginPass(login, password, new_user, num_log, row);

    if(mess.length()==0)
    {
        //Операция успешна
//        ret=false;

        QString comm="Report:";
        int num_com=12;

        _out << quint16(0);
        _out << comm;
        _out << num_com;
        _out << num_log;



    }
    else
    {
        //Ошибка


        QString comm="Error:";
        int num_com=12;

        _out << quint16(0);
        _out << comm;
        _out << num_com;
        _out << mess;

    }

    _out.device()->seek(0);
    quint16 bs=(quint16)(_out.device()->size() - sizeof(quint16));
    _out << bs;

    return false;
}
//----------------------------------------------------------
void tSaveLoginPassword::SendReport(QDataStream &)
{

}
//----------------------------------------------------------
//**********************************************************
//----------------------------------------------------------
