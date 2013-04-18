#ifndef TCALCHASH_H
#define TCALCHASH_H
#include <QCryptographicHash>
#include <QByteArray>
#include <QString>
#include <QFile>

class tCalcHash
{
public:
    tCalcHash();
    ~tCalcHash();
    void AddToHash(const QByteArray data);
    QString ResultHash() const;
    QString GetFileHash(const QString& _name) const;

private:
    QCryptographicHash *hash;
};

#endif // TCALCHASH_H
