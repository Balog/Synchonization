#ifndef TNEWPATH_H
#define TNEWPATH_H

#include <QDialog>
#include <QFileDialog>
#include "tDatabaseOp.h"

namespace Ui {
class tNewPath;
}

class tNewPath : public QDialog
{
    Q_OBJECT
    
public:
    explicit tNewPath(QWidget *parent = 0);
    ~tNewPath();
    void SetMessage(const QString &_mess);
    void SetDatabase(tDatabaseOp* _db);
    void SetLogin(const QString &_login);
    
private slots:
    void on_tbProjectFolder_clicked();

    void on_tbTempFolder_clicked();

    void on_pbCancel_clicked();

    void on_pbOk_clicked();

private:
    Ui::tNewPath *ui;
    QString project_folder;
    QString temp_folder;
    tDatabaseOp* db_op;
    QString login;
signals:
    void ContinueStrat();
};

#endif // TNEWPATH_H
