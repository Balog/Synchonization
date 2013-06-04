#ifndef AUTORIZ_H
#define AUTORIZ_H

#include <QDialog>
#include "tLog.h"

namespace Ui {
class Autoriz;
}

class Autoriz : public QDialog
{
    Q_OBJECT
    
public:
    explicit Autoriz(QWidget *_parent = 0);
    ~Autoriz();

    
private:
    Ui::Autoriz *ui;
    QString login;
    QString password;
    tLog log;


signals:
    void SendAutorization(QString& _login, QString& _password, bool _mod_fold);

public slots:
    void OnStartAutorizForm();
private slots:


    void on_pbOk_clicked();
};

#endif // AUTORIZ_H
