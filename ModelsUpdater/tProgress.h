#ifndef TPROGRESS_H
#define TPROGRESS_H

#include <QDialog>

namespace Ui {
class tProgress;
}

class tProgress : public QDialog
{
    Q_OBJECT
    
public:
    explicit tProgress(QWidget *parent = 0);
    ~tProgress();
    
private:
    Ui::tProgress *ui;

public slots:

};

#endif // TPROGRESS_H
