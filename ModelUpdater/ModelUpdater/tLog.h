#ifndef TLOG_H
#define TLOG_H
#include <QString>


class tLog
{
public:
    tLog();
    ~tLog();
    void Write(const QString &_text, const bool flag = true) const;

private:
    QString file_name;

};

#endif // TLOG_H
