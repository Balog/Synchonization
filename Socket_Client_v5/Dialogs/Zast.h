#ifndef ZAST_H
#define ZAST_H

#include <QDialog>
#include <QTimer>

#include "Autoriz.h"
#include <QMouseEvent>
#include "MainForm.h"




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
    MainForm *main_form;
    QTimer *timer1;
    QTimer *timer2;
    Autoriz *dAutoriz;
    bool connect_ok;

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private slots:
    void OnTimerTrue();
    void OnTimerFalse();
    void AutorizStart();
    void FindServer();



};

#endif // ZAST_H
