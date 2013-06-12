#ifndef TSTART_H
#define TSTART_H

#include <QDialog>
#include "../ModelUpdater/ModelUpdater/tAutoExportMain.h"

namespace Ui {
class tStart;
}

class tStart : public QDialog
{
    Q_OBJECT
    
public:
    explicit tStart(QWidget *parent = 0);
    ~tStart();
    
private slots:
    void on_pushButton_clicked();
    void OnError(QString& error);

private:
    Ui::tStart *ui;
    tAutoExportMain *main;
};

#endif // TSTART_H
