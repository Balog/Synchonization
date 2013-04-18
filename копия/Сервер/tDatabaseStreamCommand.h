#ifndef TDATABASESTREAMCOMMAND_H
#define TDATABASESTREAMCOMMAND_H
#include "tDatabaseOp.h"
#include <QtSql>

class tDatabaseStreamCommand : public tAbstractStreamCommand
{
public:

//-----------------------------------------------------

tDatabaseStreamCommand::tDatabaseStreamCommand():db_op(NULL)
{
//        db=QSqlDatabase::addDatabase("QSQLITE");
//        db.setDatabaseName("Database.db");
//        if(!db.open()){qDebug() << db.lastError().text();}

//        db_op=new tDatabaseOp(_db);
//        tDatabaseOp db_op(db);
}
//-----------------------------------------------------
tDatabaseStreamCommand::~tDatabaseStreamCommand()
{
    delete db_op;
    db_op=NULL;

//        db.close();
}
//-----------------------------------------------------
void tDatabaseStreamCommand::InitDB(QSqlDatabase &_db)
{
    db=_db;
    db_op=new tDatabaseOp(_db);
}
//-----------------------------------------------------
private:
    QSqlDatabase db;


protected:
    tDatabaseOp* db_op;

};
#endif // TDATABASESTREAMCOMMAND_H
