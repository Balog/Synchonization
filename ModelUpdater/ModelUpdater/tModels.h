#ifndef TMODELS_H
#define TMODELS_H

struct tServerFile
{
    //- File
    //- Size
    //- LastMod
    //- Hash
    //- Серверный номер файла
    //- Номер логина последнего изменявшего файл

    QString File;
    qlonglong Size;
    QDateTime LastMod_F;
    QString Hash_F;
    qlonglong Num_F;
    qlonglong num_who;
};

struct tServerModel
{
    //- Количество файлов модели

    //- DiskFile
    //- Title
    //- Description
    //- Struct
    //- LastMod
    //- Hash
    //- ListFilesLastMod
    //- ListFilesHash
    //- SummListHash
    //- Серверный номер модели

    QString DiskFile;
    QString Title;
    QString Description;
    QString Struct;
    QDateTime LastMod;
    QString Hash;
    QDateTime ListFilesLastMod;
    QString ListFilesHash;
    QString SummListHash;
    qlonglong Num;

    QList<tServerFile> Files;
};



#endif // TMODELS_H
