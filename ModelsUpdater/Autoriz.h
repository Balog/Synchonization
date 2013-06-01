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

    
private:
    Ui::Autoriz *ui;


signals:


private slots:


};

#endif // AUTORIZ_H
