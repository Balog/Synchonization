#ifndef AUTORIZ_H
#define AUTORIZ_H

#include <QDialog>

namespace Ui {
class Autoriz;
}

class Autoriz : public QDialog
{
    Q_OBJECT
    
public:
    explicit Autoriz(QWidget *_parent = 0);
    ~Autoriz();
    void GetAutorization(QString& _login, QString& _password);
    
private:
    Ui::Autoriz *ui;
    QString login;
    QString password;

signals:
    void SendAutorization(QString& _login, QString& _password, bool _mod_fold);

private slots:
    void OnOk();

};

#endif // AUTORIZ_H
