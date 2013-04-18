#ifndef TSERVERCOMMANDS_H
#define TSERVERCOMMANDS_H
#include <QFile>
#include "tAbstractCommand.h"

class tSendFile : public tAbstractCommand
{
    Q_OBJECT
public:
    tSendFile();
    ~tSendFile();
    virtual void ExeCommand();
private:

    void SendFile(QString &FileName);

signals:
    void EndCommand();
};
inline tAbstractCommand* create_tSendFile()
{
    return new tSendFile;
}

//--------------------------------------------------------------
class tReceiveFile : public tAbstractCommand
{
    Q_OBJECT
public:
    tReceiveFile();
    ~tReceiveFile();
    virtual void ExeCommand();
    virtual void ContinueRead();

private:
    void FinishReceiveFile();
    qint64 FileSize;

signals:
    void EndCommand();
};
inline tAbstractCommand* create_tReceiveFile()
{
    return new tReceiveFile;
}

#endif // TSERVERCOMMANDS_H
