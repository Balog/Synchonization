#ifndef TFILEBLOCKER_H
#define TFILEBLOCKER_H
#include "tReadWriteMutex.h"
#include <map>
#include "tLog.h"

using namespace std;

class tFileBlocker
{
public:
    tFileBlocker();//{max_readers=10;}
    ~tFileBlocker();
    void SetMaxReaders(const int _max_readers);
    void StartReadFile(const QString &_file_name, const QString &_login);
    void StartWriteFile(const QString &_file_name, const QString &_login);
    void FinishReadFile(const QString &_file_name, const QString &_login);
    void FinishWriteFile(const QString &_file_name, const QString &_login);
    bool IsExists(const QString &_file_name) const;
    void UnlockReadWrite(const QString &_login);

    tReadWriteMutex* map_blocker;


private:
    typedef map<QString, tReadWriteMutex*>block_file;
    tReadWriteMutex* SearchBlockFile(const QString &_file_name, bool &_is_found) const;
    void DeleteBlockFile(const QString &_file_name);

    block_file bf;
    int max_readers;

    tLog log;
};

#endif // TFILEBLOCKER_H
