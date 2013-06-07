#ifndef TCONSTRUCTMODELTREE_H
#define TCONSTRUCTMODELTREE_H
#include <QObject>
#include <QStringList>
#include "tExportMain.h"

class Q_DECL_EXPORT tConstructModelTree
{
public:
    tConstructModelTree(tExportMain *_main, const QString& _login);
    ~tConstructModelTree();
    bool NextModelAdmin();
    QStringList ListAdmin(bool &_read, qlonglong &_server_num);

private:
//    tConstructModelTree();

    QString table_name;
    QString login;
    tExportMain *main;
};

#endif // TCONSTRUCTMODELTREE_H
