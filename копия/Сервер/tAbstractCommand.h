#ifndef TABSTRACTCOMMAND_H
#define TABSTRACTCOMMAND_H
#include<QTcpSocket>
#include <QFile>

class tAbstractCommand : public QObject
{
    Q_OBJECT
public:
    tAbstractCommand() {}
    virtual void ExeCommand() {}
    virtual void ContinueRead() {}
    void SetRootPath(QString& Root);
    void SetSocket(QTcpSocket* socket);
    void SendErrorReport(QString& Mess);

protected:
    QString RootPath;

    QTcpSocket* socket;
    QFile *inputFile;
};

#endif // TABSTRACTCOMMAND_H
