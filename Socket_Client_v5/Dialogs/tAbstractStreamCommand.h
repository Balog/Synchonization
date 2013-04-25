#ifndef TABSTRACTSTREAMCOMMAND_H
#define TABSTRACTSTREAMCOMMAND_H
#include <QDataStream>
#include "tLog.h"

class tAbstractStreamCommand : public QObject
{
    Q_OBJECT
public:
    tAbstractStreamCommand() {}
    virtual bool Initialize(QDataStream& in) = 0;
    virtual bool ExeCommand(QDataStream& in) = 0;
    virtual void ProcessError(QDataStream& in) = 0;
    virtual void ExternalExit() = 0;

protected:
    QString l;
    tLog log;

signals:
    void EndCommand();
    void Result(QByteArray block);

};

#endif // TABSTRACTSTREAMCOMMAND_H
