#ifndef ZAST_H
#define ZAST_H

#include <QDialog>

#include "Autoriz.h"
#include <QMouseEvent>
#include "tLog.h"

#include "MainForm.h"
#include "../ModelUpdater/ModelUpdater/tExportZast.h"


namespace Ui {
class Zast;
}



class Zast : public QDialog
{
    Q_OBJECT
    
public:
    explicit Zast(QWidget *parent = 0);
    ~Zast();
//    void OnTimerTrue();
    
private:
    Ui::Zast *ui;

//    void FindServer();
    tLog log;
    MainForm *main_form;
    tExportZast* zast;

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private slots:
//    void OnTimerFalse();
    void OnFindServer(bool ok);
//    void OnFindServerTrue();
    void OnReconnect();


signals:
    void ReceiveLoginsTable();
};

#endif // ZAST_H
