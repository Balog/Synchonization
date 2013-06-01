#ifndef ZAST_H
#define ZAST_H

#include <QDialog>

#include "Autoriz.h"




namespace Ui {
class Zast;
}

class Zast : public QDialog
{
    Q_OBJECT
    
public:
    explicit Zast(QWidget *parent = 0);
    ~Zast();
    
private:
    Ui::Zast *ui;

protected:

private slots:



signals:

};

#endif // ZAST_H
