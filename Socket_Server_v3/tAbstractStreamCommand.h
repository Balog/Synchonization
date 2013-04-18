#ifndef TABSTRACTSTREAMCOMMAND_H
#define TABSTRACTSTREAMCOMMAND_H
#include <QDataStream>
#include <QObject>



class tAbstractStreamCommand : public QObject
{
    Q_OBJECT
public:
    virtual void SetLink(QObject* _link) {link=_link;}
    virtual bool Initialize(QDataStream& _in) = 0;
    virtual bool ExeCommand(QDataStream& _in, QDataStream& _out) = 0;
    virtual void SendErrorReport(QDataStream &_in) = 0;
    virtual void SendReport(QDataStream &_in) = 0;
    virtual void ExternalExit() = 0;
protected:
QObject* link;

signals:
    void EndCommand();

};

#endif // TABSTRACTSTREAMCOMMAND_H
