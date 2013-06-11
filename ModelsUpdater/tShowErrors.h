#ifndef TSHOWERRORS_H
#define TSHOWERRORS_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class tShowErrors;
}

class tShowErrors : public QDialog
{
    Q_OBJECT
    
public:
    explicit tShowErrors(QWidget *parent = 0);
    ~tShowErrors();
    void SetTextErrors(const QStringList &list);
    
private:
    Ui::tShowErrors *ui;
    QStringListModel* tableModel;
};

#endif // TSHOWERRORS_H
