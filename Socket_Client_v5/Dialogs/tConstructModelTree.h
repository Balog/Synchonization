#ifndef TCONSTRUCTMODELTREE_H
#define TCONSTRUCTMODELTREE_H
#include <QObject>
#include <QStringList>
#include "tDatabaseOp.h"

class tConstructModelTree
{
public:
    tConstructModelTree(tDatabaseOp *_db_op, QString& _table);
    ~tConstructModelTree();
    bool NextModel();
    QStringList List();

private:
    tConstructModelTree();

    QString table_name;
    tDatabaseOp *db_op;
};

#endif // TCONSTRUCTMODELTREE_H
