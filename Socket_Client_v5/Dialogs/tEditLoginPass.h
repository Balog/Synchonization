#ifndef TEDITLOGINPASS_H
#define TEDITLOGINPASS_H

#include <QDialog>

namespace Ui {
class tEditLoginPass;
}

class tEditLoginPass : public QDialog
{
    Q_OBJECT
    
public:
    explicit tEditLoginPass(QWidget *parent = 0);
    ~tEditLoginPass();

    void SetLogin(const QString &_login);
    QString GetLogin() const;
    QString GetPassword() const;
    bool new_user;
    void ClearAll();

    int row;
    
private:
    Ui::tEditLoginPass *ui;


private slots:
    void OnSave();
    void OnCancel();

signals:
    void EndEditing(QString& _login, QString& _password, int row, bool new_user);
};

#endif // TEDITLOGINPASS_H
