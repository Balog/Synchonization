#include "tReadWriteMutex.h"

tReadWriteMutex::tReadWriteMutex(const int _max):semaphore(_max), write_user_login("")
{
    max=_max;
}
//------------------------------------------------------------------------------
tReadWriteMutex::~tReadWriteMutex()
{
    us.clear();
}

//------------------------------------------------------------------------------

void tReadWriteMutex::lockRead(const QString &_login)
{
    //занимается семафор и записывается логин того кто его взял
    semaphore.acquire();
    us.push_back(_login);
}
//------------------------------------------------------------------------------

void tReadWriteMutex::unlockRead(const QString &_login)
{
    //освобождение семафора, поиск и удаление логина из списка логинов
    semaphore.release();
    user_semaphore::const_iterator it;

    for(it=us.begin();it!=us.end();it++)
    {
        if((*it)==_login)
        {
            us.erase(it);
            break;
        }
    }
}
//------------------------------------------------------------------------------

void tReadWriteMutex::lockWrite(const QString &_login)
{
//установка временного мьютекса на время занятия всех семафоров

    QMutexLocker locker(&mutex);
    for(int i=0; i<maxReaders(); i++)
    {
        semaphore.acquire();
    }

//запоминание кто взял блокировку на запись
    write_user_login=_login;
}
//------------------------------------------------------------------------------

void tReadWriteMutex::unlockWrite()
{
//возвращение всех семафоров, что были забраны (если все правильно  то максимальное количество)
    semaphore.release(maxReaders()-semaphore.available());
//Очистка сохраненного логина того кто брал блокировку на запись
    write_user_login="";
}
//------------------------------------------------------------------------------

int tReadWriteMutex::maxReaders() const
{
    return max;
}
//------------------------------------------------------------------------------

bool tReadWriteMutex::IsFree()
{
    return semaphore.available()==max;
}
//------------------------------------------------------------------------------

bool tReadWriteMutex::isWriteBlock()
{
    return write_user_login!="";
}
//------------------------------------------------------------------------------

void tReadWriteMutex::unlockReadWrite(const QString &_login)
{
    if(write_user_login==_login)
    {
        write_user_login="";
        semaphore.release(maxReaders()-semaphore.available());
    }
    user_semaphore::const_iterator it=us.begin();
    while(it!=us.end())
    {
        if((*it)==_login)
        {
            semaphore.release();
            it=us.erase(it);


        }
        else
        {
        it++;
        }

    }
}
//------------------------------------------------------------------------------
bool tReadWriteMutex::isWriteBlock(const QString &_login) const
{
    return write_user_login==_login;
}
