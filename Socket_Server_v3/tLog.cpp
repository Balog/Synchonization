#include "tLog.h"
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>


tLog::tLog():file_name("Server.log")
{
}
//---------------------------------------------------------
tLog::tLog(QString &_sender):file_name("Server.log")
{
    sender=QString::fromUtf8(_sender.toAscii());
}
//---------------------------------------------------------
tLog::tLog(char* _sender):file_name("Server.log")
{
    sender=QString::fromUtf8(_sender);
}
//---------------------------------------------------------
tLog::~tLog()
{

}
//---------------------------------------------------------
void tLog::Write(const QString &_text, const bool flag) const
{
    QFile file(file_name);
    if(file.exists() && file.open(QFile::Append))
    {
        QTextStream out(&file);

        out.setCodec("UTF-8");

        if(flag)
        {
        QDate now_date=QDate::currentDate();
        QTime now_time=QTime::currentTime();

        out << now_date.toString(Qt::ISODate) << " " << now_time.toString(Qt::ISODate) << " " << sender << " " << _text << "\r\n";
        }
        else
        {
            out << _text << "\r\n";
        }
    }

}
//---------------------------------------------------------
void tLog::Write(const char* _text, const bool flag)
{
    QString text=QString::fromUtf8(_text);
    Write(text, flag);
}
