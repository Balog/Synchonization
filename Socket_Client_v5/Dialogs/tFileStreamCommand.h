#ifndef TFILESTREAMCOMMAND_H
#define TFILESTREAMCOMMAND_H
#include "tAbstractStreamCommand.h"
#include <QFile>

class tFileStreamCommand : public tAbstractStreamCommand
{
public:
    tFileStreamCommand() {}
private:



protected:
    QString root;
    QString file_name;
    QFile file;
    qint64 file_size;
    bool error_detected;
    QString hash;
};


#endif // TFILESTREAMCOMMAND_H
