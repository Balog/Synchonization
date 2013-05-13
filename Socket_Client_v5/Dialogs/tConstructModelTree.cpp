#include "tConstructModelTree.h"

tConstructModelTree::tConstructModelTree():db_op(NULL), table_name("")
{
}
//--------------------------------------------------------------------
tConstructModelTree::~tConstructModelTree()
{
    db_op->ResetFoundModel(table_name);
}
//--------------------------------------------------------------------
tConstructModelTree::tConstructModelTree(tDatabaseOp *_db_op, QString& _table)
{
    table_name=_table;
    db_op=_db_op;

    db_op->ResetFoundModel(table_name);
}
//--------------------------------------------------------------------
bool tConstructModelTree::NextModel()
{
//    bool ret=false;
    return db_op->NextModel(table_name);
}
//--------------------------------------------------------------------
QStringList tConstructModelTree::List()
{
    return db_op->NextStructListModel(table_name);

}
//--------------------------------------------------------------------
