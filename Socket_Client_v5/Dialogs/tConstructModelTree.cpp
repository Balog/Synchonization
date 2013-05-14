#include "tConstructModelTree.h"

tConstructModelTree::tConstructModelTree():db_op(NULL), table_name("")
{
}
//--------------------------------------------------------------------
tConstructModelTree::~tConstructModelTree()
{
    db_op->ResetFoundModelAdmin();
}
//--------------------------------------------------------------------
tConstructModelTree::tConstructModelTree(tDatabaseOp *_db_op, QString &_login)
{
//    table_name=_table;
    login=_login;
    db_op=_db_op;

    db_op->ResetFoundModelAdmin();
}
//--------------------------------------------------------------------
bool tConstructModelTree::NextModelAdmin()
{
//    bool ret=false;
    return db_op->NextModelAdmin();
}
//--------------------------------------------------------------------
QStringList tConstructModelTree::ListAdmin(bool &_read, qlonglong& server_num)
{
    return db_op->NextStructListModelAdmin(login, _read, server_num);

}
//--------------------------------------------------------------------
