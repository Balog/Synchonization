#ifndef TAUTOEXPORTMAIN_H
#define TAUTOEXPORTMAIN_H

#include <QObject>
#include "MainModule.h"

class Q_DECL_EXPORT tAutoExportMain : public QObject
{
    Q_OBJECT
private:
    MainModule* MModule;
    void StartFindServer();


public:
    explicit tAutoExportMain(QObject *parent = 0);
    ~tAutoExportMain();

signals:
    void Error(QString& error);

private slots:
    void OnFindServer(bool ok);
    void OnErrorFolders(QString& error);

public slots:
    
};

#endif // TAUTOEXPORTMAIN_H
