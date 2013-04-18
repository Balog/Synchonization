#include "tCalcHash.h"

tCalcHash::tCalcHash()
{
    hash=new QCryptographicHash(QCryptographicHash::Sha1);
}
//---------------------------------------------------------------
tCalcHash::~tCalcHash()
{
    delete hash;
}
//---------------------------------------------------------------
void tCalcHash::AddToHash(const QByteArray data)
{
    hash->addData(data);
}
//---------------------------------------------------------------
QString tCalcHash::ResultHash()const
{
    QString str_hash= hash->result().toHex().data();
    return str_hash;
}
//---------------------------------------------------------------
QString tCalcHash::GetFileHash(const QString& _name) const
{

    QString str_hash="";
    QFile file(_name);
    if(file.open(QIODevice::ReadOnly))
    {

        QCryptographicHash hash(QCryptographicHash::Sha1);

        while(!file.atEnd())
        {
            QByteArray data=file.read(1024);
            hash.addData(data);
        }
        str_hash= hash.result().toHex().data();
        file.close();
    }
    return str_hash;
}
