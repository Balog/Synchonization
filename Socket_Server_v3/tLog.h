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
    void Write(QString &_text, bool flag = true);
    void Write(char* _text, bool flag = true);

private:
    QString file_name;
    QString sender;
};

#endif // TLOG_H
