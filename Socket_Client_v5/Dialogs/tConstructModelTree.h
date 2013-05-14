#ifndef TCONSTRUCTMODELTREE_H
#define TCONSTRUCTMODELTREE_H
#include <QObject>
#include <QStringList>
#include "tDatabaseOp.h"

class tConstructModelTree
{
public:
    tConstructModelTree(tDatabaseOp *_db_op, QString& _login);
    ~tConstructModelTree();
    bool NextModelAdmin();
    QStringList ListAdmin(bool &_read, qlonglong &server_num);

private:
    tConstructModelTree();

    QString table_name;
    QString login;
    tDatabaseOp *db_op;
};

#endif // TCONSTRUCTMODELTREE_H
