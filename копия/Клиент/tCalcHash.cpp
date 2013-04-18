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
void tCalcHash::AddToHash(const QByteArray _data)
{
    hash->addData(_data);
}
//---------------------------------------------------------------
QString tCalcHash::ResultHash() const
{
    QString Hash= hash->result().toHex().data();
    return Hash;
}
//---------------------------------------------------------------
QString tCalcHash::GetFileHash(const QString& _name) const
{

    QString Hash="";
    QFile file(_name);
    if(file.open(QIODevice::ReadOnly))
    {

        QCryptographicHash hash(QCryptographicHash::Sha1);

        while(!file.atEnd())
        {
            QByteArray data=file.read(1024);
            hash.addData(data);
        }
        Hash= hash.result().toHex().data();
        file.close();
    }
    return Hash;
}
