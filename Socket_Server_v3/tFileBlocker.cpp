#include "tFileBlocker.h"
#include <QDebug>

tFileBlocker::tFileBlocker()
{
    QString s="FileBlocker ";
    tLog log1(s);
    log=log1;
}
//-----------------------------------------------------------------------------------
void tFileBlocker::SetMaxReaders(const int _max_readers)
{
    max_readers=_max_readers;
    map_blocker=new tReadWriteMutex(max_readers);
}
//-----------------------------------------------------------------------------------
tFileBlocker::~tFileBlocker()
{
    delete map_blocker;
    map_blocker=NULL;
    bf.clear();
}
//-----------------------------------------------------------------------------------
tReadWriteMutex* tFileBlocker::SearchBlockFile(const QString &_file_name, bool &_is_found) const
{
    block_file::const_iterator it=bf.find(_file_name);
    if(it !=bf.end())
    {

        //элемент найден
        _is_found=true;
        qDebug() << QString::fromUtf8("Старый элемент ") << QString::fromUtf8(_file_name.toAscii());
        return (*it).second;
    }
    else
    {


        //элемент не найден
        _is_found=false;
        qDebug() << QString::fromUtf8("Новый элемент ") << QString::fromUtf8(_file_name.toAscii());
        return new tReadWriteMutex(max_readers);
    }

}
//-----------------------------------------------------------------------------------
void tFileBlocker::DeleteBlockFile(const QString& _file_name)
{
    block_file::iterator it=bf.find(_file_name);
    if(it != bf.end())
    {
        QString l=QString::fromUtf8("tFileBlocker \t SearchBlockFile \t Файл")+_file_name+QString::fromUtf8(" ничем не блокирован и удаляется из таблицы файлов");
        log.Write(l);

        delete (*it).second;
        bf.erase(it);
    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::StartReadFile(const QString &_file_name, const QString &_login)
{
    //блокировка файла на чтение
    //что бы заблокировать файл для чтения нужно сначала прочитать таблицу файлов,
    //а для этого сначала заблокировать таблицу файлов для чтения
    QString l1=QString::fromUtf8("tFileBlocker \t StartReadFile \t ==== Блокировка файла на чтение ==== ")+_file_name;
    log.Write(l1);

    map_blocker->lockRead(_login);

    QString l=QString::fromUtf8("tFileBlocker \t StartReadFile \t Таблица файлов заблокирована на чтение логином ")+_login;
    log.Write(l);

    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);

    l=QString::fromUtf8("tFileBlocker \t StartReadFile \t Таблица файлов разблокирована на чтение логином ")+_login;
    log.Write(l);

    if(is_found)
    {

        //найдено
        //если найдено то добавить блокировку на чтение для блокирующего объекта
        block_file->lockRead(_login);

        QString l=QString::fromUtf8("tFileBlocker \t StartReadFile \t Файл ")+_file_name+QString::fromUtf8(" найден в списке и добавлена блокировка на чтение логином ")+_login;
        log.Write(l);
    }
    else
    {
        QString l=QString::fromUtf8("tFileBlocker \t StartReadFile \t Таблица файлов заблокирована на запись логином ")+_login;
        log.Write(l);
        //не найдено
        //если не найдено, нужно добавить запись в таблицу, установить блокировку на запись
        map_blocker->lockWrite(_login);

        //добавить блокировку на чтение к новому файлу

       l=QString::fromUtf8("tFileBlocker \t StartReadFile \t Файл ")+_file_name+QString::fromUtf8(" добавление в список и блокировка на чтение логином ")+_login;
        log.Write(l);

        block_file->lockRead(_login);
        //записать новую запись в таблицу
        bf.insert(block_file::value_type(_file_name, block_file));


        //освободить таблицу от блокировки записи

        map_blocker->unlockWrite();
        l=QString::fromUtf8("tFileBlocker \t StartReadFile \t Таблица файлов разблокирована на запись логином ")+_login;
        log.Write(l);

    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::StartWriteFile(const QString& _file_name, const QString &_login)
{
    QString l1=QString::fromUtf8("tFileBlocker \t StartWriteFile \t ==== Блокировка файла на запись ==== ")+_file_name;
    log.Write(l1);
//блокировка файла на запись
    //для того что бы заблокировать файл на запись
    //нужно для начала найти файл в списке файлов
    //а для этого сначала закрыть таблицу на чтение
    QString l=QString::fromUtf8("tFileBlocker \t StartWriteFile \t Таблица файлов заблокирована на чтение логином ")+_login;
    log.Write(l);

    map_blocker->lockRead(_login);
    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы

    map_blocker->unlockRead(_login);

    l=QString::fromUtf8("tFileBlocker \t StartWriteFile \t Таблица файлов разблокирована на чтение логином ")+_login;
    log.Write(l);

    if(is_found)
    {
;

        //если файл в таблице найден заблокировать файл на запись
        block_file->lockWrite(_login);

        QString l=QString::fromUtf8("tFileBlocker \t StartWriteFile \t Файл ")+_file_name+QString::fromUtf8(" найден в списке и добавлена блокировка на запись логином ")+_login;
        log.Write(l);

    }
    else
    {
        //если файл не найден, то сначала нужно заблокировать таблицу файлов на запись
        QString l=QString::fromUtf8("tFileBlocker \t StartWriteFile \t Таблица файлов заблокирована на запись логином ")+_login;
        log.Write(l);

        map_blocker->lockWrite(_login);

        //добавить блокировку на чтение к новому файлу

        l=QString::fromUtf8("tFileBlocker \t StartWriteFile \t Файл ")+_file_name+QString::fromUtf8(" добавление в список и блокировка на запись логином ")+_login;
         log.Write(l);

        block_file->lockWrite(_login);
        //записать новую запись в таблицу
        bf.insert(block_file::value_type(_file_name, block_file));

        //разблокировать таблицу от записи
        map_blocker->unlockWrite();

        l=QString::fromUtf8("tFileBlocker \t StartWriteFile \t Таблица файлов разблокирована на запись логином ")+_login;
        log.Write(l);
    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::FinishReadFile(const QString& _file_name, const QString &_login)
{
    QString l1=QString::fromUtf8("tFileBlocker \t FinishReadFile \t ==== Снятие блокировки на чтение ==== ")+_file_name;
    log.Write(l1);

//снятие блокировки на чтение с файла
    //для того что бы снять блокировку на чтение нужно найти файл в таблице файлов
    //а для этого заблокировать ее на чтение
    QString l=QString::fromUtf8("tFileBlocker \t FinishReadFile \t Таблица файлов заблокирована на чтение логином ")+_login;
    log.Write(l);

    map_blocker->lockRead(_login);
    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);

    l=QString::fromUtf8("tFileBlocker \t FinishReadFile \t Таблица файлов разблокирована на чтение логином ")+_login;
    log.Write(l);

    if(is_found)
    {


        //если файл найден то снимаем с него блокировку на чтение
        block_file->unlockRead(_login);

        QString l=QString::fromUtf8("tFileBlocker \t FinishReadFile \t Файл ")+_file_name+QString::fromUtf8(" найден в списке и снята блокировка на чтение логином ")+_login;
        log.Write(l);

        //проверяем блокирует ли его еще кто-то и если не блокирует то удаляем запись
        if(block_file->IsFree())
        {
            DeleteBlockFile(_file_name);
        }
    }
    else
    {
        //если файл не найден
        //то нужно удалить созданый блокирующий объект
        qDebug() << QString::fromUtf8("Освобождаемый от записи файл не найден в таблице файлов (чтение) FileName: ") << QString::fromUtf8(_file_name.toAscii()) << "Login: " << QString::fromUtf8(_login.toAscii());
        delete block_file;

        QString l=QString::fromUtf8("tFileBlocker \t FinishReadFile \t Разблокируемый файл ")+_file_name+QString::fromUtf8(" не найден в списке ТАКОГО БЫТЬ НЕ ДОЛЖНО!!! Логин: ")+_login;
        log.Write(l);
    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::FinishWriteFile(const QString& _file_name, const QString &_login)
{
    QString l1=QString::fromUtf8("tFileBlocker \t FinishWriteFile \t ==== Снятие блокировки на запись ==== ")+_file_name;
    log.Write(l1);
    //снятие блокировки на запись
    //для того что бы снять блокировку на чтение нужно найти файл в таблице файлов
    //а для этого заблокировать ее на чтение
    QString l=QString::fromUtf8("tFileBlocker \t FinishWriteFile \t Таблица файлов заблокирована на чтение логином ")+_login;
    log.Write(l);

    map_blocker->lockRead(_login);

    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);

    l=QString::fromUtf8("tFileBlocker \t FinishWriteFile \t Таблица файлов разблокирована на чтение логином ")+_login;
    log.Write(l);

    if(is_found)
    {
        //если файл найден то снимаем с него блокировку на запись
        block_file->unlockWrite();

        QString l=QString::fromUtf8("tFileBlocker \t FinishWriteFile \t Файл ")+_file_name+QString::fromUtf8(" найден в списке и снята блокировка на запись логином ")+_login;
        log.Write(l);

        //проверять использует ли его кто-то нет смысла, потому как после блокировки на запись его никто кроме нас не использует
        //потому удаляем сразу

            DeleteBlockFile(_file_name);

    }
    else
    {
        //если файл не найден
        //то нужно удалить созданый блокирующий объект
        qDebug() << QString::fromUtf8("Освобождаемый от записи файл не найден в таблице файлов (запись) FileName: ") << QString::fromUtf8(_file_name.toAscii()) << "Login: " << QString::fromUtf8(_login.toAscii());
        delete block_file;

        QString l=QString::fromUtf8("tFileBlocker \t FinishWriteFile \t Разблокируемый файл ")+_file_name+QString::fromUtf8(" не найден в списке ТАКОГО БЫТЬ НЕ ДОЛЖНО!!! Логин: ")+_login;
        log.Write(l);
    }
}
//-----------------------------------------------------------------------------------
bool tFileBlocker::IsExists(const QString &_file_name) const
{
    block_file::const_iterator it=bf.find(_file_name);
    return it != bf.end() && (*it).second->isWriteBlock();
}
//-----------------------------------------------------------------------------------
void tFileBlocker::UnlockReadWrite(const QString &_login)
{
    QString l=QString::fromUtf8("tFileBlocker \t UnlockReadWrite \t === Снятие блокировок со всех файлов на чтение и запись логина === ")+_login;
    log.Write(l);

    //снять блокировку на запись и чтение указаному логину со всех файлов в таблице
    //удалить те записи что уже не используются
    //сначала перед поиском по таблице нужно заблокировать таблицу на чтение

    //проверить не блокируем ли мы таблицу от записи
    if(map_blocker->isWriteBlock(_login))
    {
        //если блокируем то снять блокировку
        map_blocker->unlockWrite();

        QString l=QString::fromUtf8("tFileBlocker \t UnlockReadWrite \t Блокировка таблицы файлов на запись снята. Логин: ")+_login;
        log.Write(l);
    }

    //заблокируем таблицу на запись

    map_blocker->lockWrite(_login);

    l=QString::fromUtf8("tFileBlocker \t UnlockReadWrite \t Блокировка таблицы файлов на запись установлена. Логин: ")+_login;
    log.Write(l);

    block_file::const_iterator it=bf.begin();
    while(it!=bf.end())
    {
        (*it).second->unlockReadWrite(_login);

        if(!(*it).second->isWriteBlock() && (*it).second->IsFree())
        {
        delete (*it).second;
        it= bf.erase(it);
        }
        else
        {
            it++;
        }
    }
//bf.clear();
    //удалим все блокировки на чтение и запись данного логина
    map_blocker->unlockReadWrite(_login);

    l=QString::fromUtf8("tFileBlocker \t UnlockReadWrite \t Блокировка таблицы файлов на чтение и запись снята. Логин: ")+_login;
    log.Write(l);
}
//-----------------------------------------------------------------------------------
