#include "tFileBlocker.h"
#include <QDebug>

tFileBlocker::tFileBlocker()
{

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
    map_blocker->lockRead(_login);
    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);
    if(is_found)
    {
        //найдено
        //если найдено то добавить блокировку на чтение для блокирующего объекта
        block_file->lockRead(_login);
    }
    else
    {
        //не найдено
        //если не найдено, нужно добавить запись в таблицу, установить блокировку на запись
        map_blocker->lockWrite(_login);

        //добавить блокировку на чтение к новому файлу
        block_file->lockRead(_login);
        //записать новую запись в таблицу
        bf.insert(block_file::value_type(_file_name, block_file));

        //освободить таблицу от блокировки записи
        map_blocker->unlockWrite();
    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::StartWriteFile(const QString& _file_name, const QString &_login)
{
//блокировка файла на запись
    //для того что бы заблокировать файл на запись
    //нужно для начала найти файл в списке файлов
    //а для этого сначала закрыть таблицу на чтение
    map_blocker->lockRead(_login);
    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);
    if(is_found)
    {
        //если файл в таблице найден заблокировать файл на запись
        block_file->lockWrite(_login);
    }
    else
    {
        //если файл не найден, то сначала нужно заблокировать таблицу файлов на запись
        map_blocker->lockWrite(_login);

        //добавить блокировку на чтение к новому файлу
        block_file->lockWrite(_login);
        //записать новую запись в таблицу
        bf.insert(block_file::value_type(_file_name, block_file));

        //разблокировать таблицу от записи
        map_blocker->unlockWrite();
    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::FinishReadFile(const QString& _file_name, const QString &_login)
{
//снятие блокировки на чтение с файла
    //для того что бы снять блокировку на чтение нужно найти файл в таблице файлов
    //а для этого заблокировать ее на чтение
    map_blocker->lockRead(_login);
    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);
    if(is_found)
    {
        //если файл найден то снимаем с него блокировку на чтение
        block_file->unlockRead(_login);
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
    }
}
//-----------------------------------------------------------------------------------
void tFileBlocker::FinishWriteFile(const QString& _file_name, const QString &_login)
{
    //снятие блокировки на запись
    //для того что бы снять блокировку на чтение нужно найти файл в таблице файлов
    //а для этого заблокировать ее на чтение
    map_blocker->lockRead(_login);
    //поиск файла в таблице. Если запись найдена то возвращается блокирующий объект из этой строке
    //Если не найден то создается новый блокирующий объект
    bool is_found=false;
    tReadWriteMutex* block_file=SearchBlockFile(_file_name, is_found);
    //снять блокировку на чтение с таблицы
    map_blocker->unlockRead(_login);
    if(is_found)
    {
        //если файл найден то снимаем с него блокировку на запись
        block_file->unlockWrite();
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
    //снять блокировку на запись и чтение указаному логину со всех файлов в таблице
    //удалить те записи что уже не используются
    //сначала перед поиском по таблице нужно заблокировать таблицу на чтение

    //проверить не блокируем ли мы таблицу от записи
    if(map_blocker->isWriteBlock(_login))
    {
        //если блокируем то снять блокировку
        map_blocker->unlockWrite();
    }

    //заблокируем таблицу на запись

    map_blocker->lockWrite(_login);

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
}
//-----------------------------------------------------------------------------------
