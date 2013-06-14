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
    QStringList ModelsList;

public:
    explicit tAutoExportMain(QObject *parent = 0);
    ~tAutoExportMain();
    void VerifyUserFolders();
    void OnListFiles();
    void Autorization(QString &_login, QString &_password);
    QStringList ReadAutoUserModels();
    void ReceivingModels(QList<tServerModel> &_models);

signals:
    void Error(QString& error);
    void FindServer(QString& _login, QString& _password);
    void FoldersOk();
    void SendModels(QList<tServerModel>& _Model);//
    void SendAutorization(QString& _login, QString& _password, bool _mod_folder);
    void EndTransactions();

private slots:
    void OnFindServer(bool ok);
    void OnErrorFolders(QString& error);
    void OnFoldersOk();
    void OnSendModels(QList<tServerModel> &_Model);
    void OnEndTransactions();

public slots:
    
};

#endif // TAUTOEXPORTMAIN_H
