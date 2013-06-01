#ifndef TNEWPATH_H
#define TNEWPATH_H

#include <QDialog>

namespace Ui {
class tNewPath;
}

class tNewPath : public QDialog
{
    Q_OBJECT
    
public:
    explicit tNewPath(QWidget *parent = 0);
    ~tNewPath();
    
private slots:


private:
        Ui::tNewPath *ui;

signals:

};

#endif // TNEWPATH_H
