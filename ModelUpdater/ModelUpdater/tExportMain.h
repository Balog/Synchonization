#ifndef TEXPORTMAIN_H
#define TEXPORTMAIN_H

#include <QObject>
#include "MainModule.h"

class Q_DECL_EXPORT tExportMain : public QObject
{
    Q_OBJECT
public:
    explicit tExportMain(QObject *parent = 0);
    ~tExportMain();

private:
    MainModule* MModule;
    
signals:
    
public slots:
    
};

#endif // TEXPORTMAIN_H
