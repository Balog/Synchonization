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
//    void OnSave();
//    void OnCancel();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

public slots:
    void OnShowEditLogin(bool Visible, bool Modal);

signals:
    void EndEditing(QString& _login, QString& _password, int row, bool new_user);
};
#endif // TEDITLOGINPASS_H
