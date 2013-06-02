#ifndef TEXPORTZAST_H
#define TEXPORTZAST_H

#include <QObject>
#include "tZastModule.h"

class Q_DECL_EXPORT tExportZast : public QObject
{
    Q_OBJECT
public:
    explicit tExportZast(QObject *parent = 0);
    ~tExportZast();
    bool GetConnect();

private:
    tZastModule* zast_mod;

    
signals:
    void FindServerFalse();
    
public slots:
    void OnFindServerFalse();
    
};

#endif // TEXPORTZAST_H
