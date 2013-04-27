#ifndef TLOG_H
#define TLOG_H
#include <QString>


class tLog
{
public:
    tLog();
    ~tLog();
    void Write(QString &_text, bool flag = true);

private:
    QString file_name;

};

#endif // TLOG_H
