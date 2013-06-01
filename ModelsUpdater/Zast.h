#ifndef ZAST_H
#define ZAST_H

#include <QDialog>

#include "Autoriz.h"
#include <QMouseEvent>
#include "tLog.h"
#include <QTimer>
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
    void OnTimerTrue();
    
private:
    Ui::Zast *ui;
    bool connect_ok;
    QTimer *timer1;
    QTimer *timer2;
    void FindServer();
    tLog log;
    MainForm *main_form;

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private slots:
    void OnTimerFalse();


signals:
    void ReceiveLoginsTable();
};

#endif // ZAST_H
