#ifndef TLOG_H
#define TLOG_H
#include <QString>


class tLog
{
public:
    tLog();
    tLog(QString &_sender);
    tLog(char* _sender);
    ~tLog();
    void Write(const QString &_text, const bool flag = true) const;
    void Write(const char *_text, const bool flag = true);

private:
    QString file_name;
    QString sender;
};

#endif // TLOG_H
