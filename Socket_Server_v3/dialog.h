#ifndef DIALOG_H
#define DIALOG_H

#include <QtCore>
#include <QDialog>
#include "Server.h"
//#include "tDatabaseOp.h"


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *_parent = 0);
    ~Dialog();
    
private:
    Ui::Dialog *ui;
    tServer* server;
    tDatabaseOp* base_op;


private slots:
    void OnConnect();
    void OnDisconnect();
    void OnConnectClient(int _handle, int _count_client);
    void OnAddNew();
    void OnClearModels();

};

#endif // DIALOG_H
