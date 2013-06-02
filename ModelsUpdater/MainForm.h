#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDialog>
#include "Zast.h"
#include "../ModelUpdater/ModelUpdater/tExportMain.h"
#include "Autoriz.h"

namespace Ui {
class MainForm;
}

class MainForm : public QDialog
{
    Q_OBJECT


public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private:
    Ui::MainForm *ui;
    tExportMain *main;
    Zast *zast;
    Autoriz* autoriz;

private slots:
    void OnFindServer(bool ok);

public slots:

private slots:



signals:
    void FindServer(bool ok);
    void ZastVisible(bool vis);

};

#endif // MAINFORM_H
