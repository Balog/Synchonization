#include "tShowErrors.h"
#include "ui_tShowErrors.h"
#include<QAbstractItemView>

tShowErrors::tShowErrors(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tShowErrors), tableModel(new QStringListModel)
{
    ui->setupUi(this);
}
//---------------------------------------------------
tShowErrors::~tShowErrors()
{
    delete tableModel;
    delete ui;
}
//---------------------------------------------------
void tShowErrors::SetTextErrors(const QStringList &list)
{
//    EditTriggers triggers;
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableModel->setStringList(list);
    ui->listView->setModel(tableModel);
}
