#ifndef TSTART_H
#define TSTART_H

#include <QDialog>
#include "../ModelUpdater/ModelUpdater/tAutoExportMain.h"
#include "../ModelUpdater/ModelUpdater/tModels.h"

namespace Ui {
class tStart;
}

class tStart : public QDialog
{
    Q_OBJECT
    
public:
    explicit tStart(QWidget *parent = 0);
    ~tStart();
    
private slots:
    void on_pushButton_clicked();
    void OnError(QString& error);
    void OnFindServer(QString &_login, QString &_password);
    void OnFoldersOk();
    void OnSendModels(QList<tServerModel> model);

private:
    Ui::tStart *ui;
    tAutoExportMain *main;
    QList<tServerModel>Models;
    QString login;
    QString password;
};

#endif // TSTART_H
