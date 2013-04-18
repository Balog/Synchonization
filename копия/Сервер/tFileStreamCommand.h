#ifndef TFILESTRAMCOMMAND_H
#define TFILESTRAMCOMMAND_H
#include "tAbstractStreamCommand.h"
#include <QFile>

class tFileStreamCommand : public tAbstractStreamCommand
{
public:
    ~tFileStreamCommand() {file.close();}

protected:
    QString root;
    QString file_name;
    qint64 file_size;
    QFile file;
    QString hash;

};
#endif // TFILESTRAMCOMMAND_H
