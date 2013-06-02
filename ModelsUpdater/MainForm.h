#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDialog>
#include "../ModelUpdater/ModelUpdater/tExportMain.h"

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

private slots:

public slots:


signals:

};

#endif // MAINFORM_H
