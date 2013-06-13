#ifndef TAUTOEXPORTMAIN_H
#define TAUTOEXPORTMAIN_H

#include <QObject>
#include "MainModule.h"
#include"tModels.h"

class Q_DECL_EXPORT tAutoExportMain : public QObject
{
    Q_OBJECT
private:
    MainModule* MModule;
    void StartFindServer();
    QList<tServerModel> Models;

public:
    explicit tAutoExportMain(QObject *parent = 0);
    ~tAutoExportMain();
    void VerifyUserFolders();
    void OnListFiles();
    void Autorization(QString &_login, QString &_password);

signals:
    void Error(QString& error);
    void FindServer(QString& _login, QString& _password);
    void FoldersOk();
    void SendModels(QList<tServerModel> Models);
    void SendAutorization(QString& _login, QString& _password, bool _mod_folder);

private slots:
    void OnFindServer(bool ok);
    void OnErrorFolders(QString& error);
    void OnFoldersOk();
    void OnSendModels(QList<tServerModel> Model);

public slots:
    
};

#endif // TAUTOEXPORTMAIN_H
