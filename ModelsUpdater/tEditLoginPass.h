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


    
private:
    Ui::tEditLoginPass *ui;


private slots:


signals:

};

#endif // TEDITLOGINPASS_H
