#ifndef TREADWRITEMUTEX_H
#define TREADWRITEMUTEX_H

#include <QSemaphore>
#include <QMutex>
#include <vector>
#include<QDebug>

using namespace std;

class tReadWriteMutex
{
public:
    tReadWriteMutex(const int _max);
    ~tReadWriteMutex();

    void lockRead(const QString &_login);
    void unlockRead(const QString &_login);
    void lockWrite(const QString &_login);
    void unlockWrite();
    bool IsFree();
    bool isWriteBlock();
    void unlockReadWrite(const QString &_login);
    int maxReaders() const;
    bool isWriteBlock(const QString &_login) const;
private:
    int max;
    QSemaphore semaphore;
    QMutex mutex;
    QString write_user_login;

    typedef vector<QString>user_semaphore; //список логинов клиентов занявших семафор. Для возвращения при неожиданном завершении клиента
    user_semaphore us;
};

#endif // TREADWRITEMUTEX_H
