#ifndef TNEWPATH_H
#define TNEWPATH_H

#include <QDialog>
#include <QFileDialog>
#include "../ModelUpdater/ModelUpdater/tExportMain.h"

namespace Ui {
class tNewPath;
}

class tNewPath : public QDialog
{
    Q_OBJECT
    
public:
    explicit tNewPath(QWidget *parent = 0);
    ~tNewPath();
    
public slots:
    void Visible(QString& _login, QString& _message, tExportMain *_main);
private slots:


    void on_tbProjectFolder_clicked();

    void on_tbTempFolder_clicked();

    void on_pbOk_clicked();

    void on_pbCancel_clicked();

private:
        Ui::tNewPath *ui;
        QString login;
        tExportMain *main;
        QString project_folder;
        QString temp_folder;

signals:
        void ContinueStrat();

};

#endif // TNEWPATH_H
