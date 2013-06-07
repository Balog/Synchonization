#include "tConstructModelTree.h"

//tConstructModelTree::tConstructModelTree():main(NULL), table_name("")
//{
//}
//--------------------------------------------------------------------
tConstructModelTree::~tConstructModelTree()
{
    main->ResetFoundModelAdmin();
}
//--------------------------------------------------------------------
tConstructModelTree::tConstructModelTree(tExportMain *_main, const QString &_login)
{
//    table_name=_table;
    login=_login;
    main=_main;

    main->ResetFoundModelAdmin();
}
//--------------------------------------------------------------------
bool tConstructModelTree::NextModelAdmin()
{
//    bool ret=false;
    return main->NextModelAdmin();
}
//--------------------------------------------------------------------
QStringList tConstructModelTree::ListAdmin(bool &_read, qlonglong& _server_num)
{
    return main->NextStructListModelAdmin(login, _read, _server_num);

}
//--------------------------------------------------------------------
