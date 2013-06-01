#ifndef MAINFORM_H
#define MAINFORM_H

#include <QDialog>
#include "../ModelUpdater/ModelUpdater/MainModule.h"

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
    MainModule *MModule;

private slots:

public slots:
    void OnProba();

signals:

};

#endif // MAINFORM_H
