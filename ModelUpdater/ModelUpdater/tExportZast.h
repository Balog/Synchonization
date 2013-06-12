#ifndef TEXPORTZAST_H
#define TEXPORTZAST_H

#include <QObject>
//#include "tZastModule.h"
#include"tLog.h"

class tExportZast : public QObject
{
    Q_OBJECT
public:
    explicit tExportZast(QObject *parent = 0);
    ~tExportZast();
    bool GetConnect();
    void StartFindServer();

private:
//    tZastModule* zast_mod;
    tLog log;
    
signals:
    void FindServer(bool ok);
//    void FindServerTrue();
    
public slots:
    void OnFindServer(bool ok);
//    void OnServerFinding();

};

#endif // TEXPORTZAST_H
