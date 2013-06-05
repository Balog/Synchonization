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
    void Start(int max, int _x, int _y, int _w, int _h);
    void Stop();
    void setValue(int _value);

};

#endif // TPROGRESS_H
