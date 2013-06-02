#ifndef TEXPORTMAIN_H
#define TEXPORTMAIN_H

#include <QObject>
#include "MainModule.h"
#include "tZastModule.h"

class Q_DECL_EXPORT tExportMain : public QObject
{
    Q_OBJECT
public:
    explicit tExportMain(QObject *parent = 0);
    ~tExportMain();

private:
    MainModule* MModule;
//    tZastModule* zast_mod;
    void StartFindServer();
    
signals:
    void FindServer(bool ok);
    void StartAutorizForm();
    
public slots:
    void OnFindServer(bool ok);

private slots:
    void OnStartAutorizForm();
    
};

#endif // TEXPORTMAIN_H
