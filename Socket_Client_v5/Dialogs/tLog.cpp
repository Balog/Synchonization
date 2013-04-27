#include "tLog.h"
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>


tLog::tLog():file_name("Client.log")
{
}
//---------------------------------------------------------
tLog::~tLog()
{

}
//---------------------------------------------------------
void tLog::Write(QString &_text, bool flag)
{
    QFile file(file_name);
    if(file.exists() && file.open(QFile::Append))
    {
        QTextStream out(&file);

        out.setCodec("Windows-1251");

        if(flag)
        {
        QDate now_date=QDate::currentDate();
        QTime now_time=QTime::currentTime();

        out << now_date.toString(Qt::ISODate) << " " << now_time.toString(Qt::ISODate) << " " << _text << "\r\n";
        }
        else
        {
            out << _text << "\r\n";
        }
    }

}
//---------------------------------------------------------
