#include "tDatabaseOp.h"
#include "tSettings.h"
#include <QMessageBox>
#include "tCalcHash.h"

extern tSettings my_settings;
//----------------------------------------------------------------------
tDatabaseOp::tDatabaseOp(const QSqlDatabase &_db)
{
    tLog log1("DB");
    log=log1;

    db=_db;
    root=my_settings.GetRoot();
//    db=QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("Database.db");
//    if(!db.open()){qDebug() << db.lastError().text();}
}
//----------------------------------------------------------------------
tDatabaseOp::~tDatabaseOp()
{
//    db.close();
}
//----------------------------------------------------------------------
void tDatabaseOp::RefreshModelsFiles()
{
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t RefreshModelsFiles \t Проверка и обновление серверной базы данных ")));

    QString root=my_settings.GetRoot();

    //Начало работы с базой данных
    //Начало транзакции
//    bool b=db.transaction();

    QSqlQuery unset_found_models(db);
    unset_found_models.prepare("UPDATE StructModels SET Found=0");

    if(!unset_found_models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при сбросе Found моделей ");
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t RefreshModelsFiles \t +++ ОШИБКА +++ при сбросе Found моделей ")));}

    QSqlQuery unset_found_files(db);
    unset_found_files.prepare("UPDATE Files SET Found=0, Del=0");

    if(!unset_found_files.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при сбросе Found и Del файлов ");
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t RefreshModelsFiles \t +++ ОШИБКА +++ при сбросе Found и Del файлов ")));}

    SearchInfo(root);

    //После обработки всех файлов и всей базы можно удалить из базы более не нужные модели у которых Found=0 (они не найдены на диске)
    QSqlQuery delete_unfound_models(db);
    delete_unfound_models.prepare("DELETE FROM StructModels WHERE Found=0");
    if(!delete_unfound_models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ удалении не найденых на диске моделей ");
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t RefreshModelsFiles \t +++ ОШИБКА +++ удалении не найденых на диске моделей ")));}

    QSqlQuery delete_unfound_files(db);
    delete_unfound_files.prepare("DELETE FROM Files WHERE Found=0 OR Del=1");
    if(!delete_unfound_files.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ удалении не найденых на диске файлов ");
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t RefreshModelsFiles \t +++ ОШИБКА +++ удалении не найденых на диске файлов ")));}

    //Окончание работы с базой данных
    //Коммит изменений
//    b=db.commit();

//    QMessageBox mb;
//    mb.setText(QString::fromUtf8("Обновление завершено"));
//    mb.setWindowTitle(QString::fromUtf8("Обновление"));
//    mb.exec();

}
//----------------------------------------------------------------------
void tDatabaseOp::SearchInfo(const QString &_folder)
{
    QSqlQuery struct_models(db);

    struct_models.prepare("INSERT INTO StructModels (DiskFile, Title, Description, Struct, LastMod, Hash, Found)"
                          "VALUES(?, ?, ?, ?, ?, ?, ?);");

    QDir dir(_folder);
    //Получаем список каталогов
    QStringList lst_dirs = dir.entryList(QDir::Dirs |
                                         QDir::AllDirs |
                                         QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = dir.entryList(QDir::Files | QDir::Hidden);

    //ищем среди файлов те что оканчиваются на .info
    foreach(QString entry, lst_files)
    {
        if(entry.right(5)==".info")
        {
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t Найден файл описания модели ")+NormalizePathFiles(entry)));
            //проверяем есть ли такое в базе
            QString relat=_folder.right(_folder.length()-root.length());
            QSqlQuery search_file(db);
            QString S="SELECT count(*) FROM StructModels WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
            search_file.prepare(S);

            if(!search_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при поиске ") << NormalizePathFiles(relat+"/"+entry);
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при поиске ")+NormalizePathFiles(relat+"/"+entry)));}

            search_file.next();

            QFileInfo info(_folder+"/"+entry);
            QDateTime last_modified=info.lastModified();


            if(search_file.value(0).toInt()==0)
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t Такого файла нет в базе ")+NormalizePathFiles(relat+"/"+entry)));
                //Такой модели в базе нет
                //заносим найденую модель в базу
                tCalcHash ch;

                QString hash=ch.GetFileHash(_folder+"/"+entry);

                QSettings s(_folder+"/"+entry, QSettings::IniFormat);
                QTextCodec *codec =QTextCodec::codecForName("UTF-8");
                s.setIniCodec(codec);

                QString title=s.value("Title","").toString();
                QString desc=s.value("Description","").toString();
                QString struct_mod=s.value("Struct","").toString();


                struct_models.bindValue(0, relat+"/"+entry);
                struct_models.bindValue(1, title);
                struct_models.bindValue(2, desc);
                struct_models.bindValue(3, struct_mod+"/"+title);
                struct_models.bindValue(4, last_modified);
                struct_models.bindValue(5, hash);
                struct_models.bindValue(6, 1);//Found

                QDateTime list_files_date_time;
                QString list_files_hash="";
                QString summ_list_hash="";

                if(!struct_models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при добавлении данных ") << relat+"/"+entry;
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при добавлении данных ")+NormalizePathFiles(relat+"/"+entry)));}

                //добавление списка файлов модели
                //передаем значение автоинкрементного ключа

                AddModelFiles(struct_models.lastInsertId().toLongLong(), _folder, entry, list_files_date_time, list_files_hash, summ_list_hash);

                //Добавляем дату-время файла модели, хеш сумму самого файла модели и сумму сумм файлов в ней описаных
                QSqlQuery update_struct_models(db);

                QString dt=list_files_date_time.toString(Qt::ISODate);

                QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+list_files_hash+"', SummListHash='"+summ_list_hash+"' WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                update_struct_models.prepare(sql);

                if(!update_struct_models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при обновлении новых данных ") << relat+"/"+entry;
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при обновлении новых данных ")+NormalizePathFiles(relat+"/"+entry)));}
            }
            else
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t Такая модель в базе есть ")+NormalizePathFiles(relat+"/"+entry)));

                //Такая модель в базе есть

                //Необходимо проверить а есть ли такая модель на диске
                QDateTime round_dt;
                QFileInfo info_mod(_folder+"/"+entry);
                if(info_mod.exists())
                {
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t Такая модель на диске есть ")+NormalizePathFiles(relat+"/"+entry)));
                    //модель на диске есть
                    //проверим время редактирования


                    QString S="SELECT lastMod, Hash, Num FROM StructModels WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                    search_file.prepare(S);

                    if(!search_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при поиске ") << NormalizePathFiles(relat+"/"+entry);
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при поиске ")+NormalizePathFiles(relat+"/"+entry)));}
                    search_file.next();

                    QDateTime base_date_time=search_file.value(0).toDateTime();
                    QString list_files_hash=search_file.value(1).toString();
                    qlonglong num_model=search_file.value(2).toLongLong();

                    round_dt=RoundDateTime(last_modified);

                    if(base_date_time!=round_dt)
                    {
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t Времена совпадают, файл не менялся ")+NormalizePathFiles(relat+"/"+entry)));
                        //читаем Title модели, описание и структуру

                        QSettings s(_folder+"/"+entry, QSettings::IniFormat);
                        QTextCodec *codec =QTextCodec::codecForName("UTF-8");
                        s.setIniCodec(codec);

                        QString title=s.value("Title","").toString();
                        QString desc=s.value("Description","").toString();
                        QString struct_mod=s.value("Struct","").toString();

                        //если время отличается, пересчитываем хэш и изменяем время и хеш в базе
                        //и корректируем все данные
                        qDebug() << relat+"/"+entry << QString::fromUtf8("отличается");
                        QString dt1=round_dt.toString(Qt::ISODate);

                        tCalcHash ch;
                        QString hash1=ch.GetFileHash(_folder+"/"+entry);

                        QString s1="UPDATE StructModels SET Title='"+title+"', Struct='"+struct_mod+"/"+title+"', Description= :desc_mod, lastMod='"+dt1+"', Hash='"+hash1+"' WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                        search_file.prepare(s1);
                        search_file.bindValue(":desc_mod", desc);

                        if(!search_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при редактировании ") << NormalizePathFiles(relat+"/"+entry);
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при редактировании ")+NormalizePathFiles(relat+"/"+entry)));}

                    }
                    //Если такая модель в базе есть то независимо от того менялся ли список файлов
                    //нужно проверить их наличие и не изменились ли они

                    //Проверить список файлов и если есть различия - откорректировать базу
                    //Эти таблицы только для ускорения работы что бы не считать хэш каждый раз

                    //Определим дату-время модификации списка файлов модели

                    QString info_file=_folder+"/"+entry;
                    QString model_structs=info_file.left(info_file.length()-4)+"files";
                    QFileInfo info_mod_file(model_structs);

                    tCalcHash l_f_hash;
                    QString u_list_file_hash=l_f_hash.GetFileHash(model_structs);

                    QDateTime list_files_date_time=info_mod_file.lastModified();

                    QString summ_list_hash="";

                    UpdateModelFiles(num_model, _folder, entry, summ_list_hash);

                    //обновить запись в таблице моделей по дате модификации, хеш-сумме и общей хеш-сумме

                    QSqlQuery update_struct_models(db);

                    QString dt=list_files_date_time.toString(Qt::ISODate);

                    QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+u_list_file_hash+"', SummListHash='"+summ_list_hash+"', Found=1 WHERE Num="+QString::number(num_model);
                    update_struct_models.prepare(sql);

                    if(!update_struct_models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при обновлении имеющихся данных ") << relat+"/"+entry;
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при обновлении имеющихся данных ")+NormalizePathFiles(relat+"/"+entry)));}


                }
                else
                {
                    //модели на диске уже нет
                    //поставить пометку Found=false
                    QSqlQuery update_found_false(db);
                    update_found_false.prepare("UPDATE StructModels SET Found=0 Where DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'");
                    if(!update_found_false.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при пометки на удаление модели ") << NormalizePathFiles(relat+"/"+entry);
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t SearchInfo \t +++ ОШИБКА +++ при пометки на удаление модели ")+NormalizePathFiles(relat+"/"+entry)));}


                }
            }
        }
    }

    //для папок делаем рекурсивный вызов
    foreach(QString entry, lst_dirs)
    {
        QString entry_abs_path = dir.absolutePath() + "/" + entry;
        SearchInfo(entry_abs_path);
    }
}
//----------------------------------------------------------
void tDatabaseOp::AddModelFiles(const qlonglong _num, const QString _path, QString _info_file, QDateTime &_list_date_time, QString &_list_files_hash, QString &_summ_list_hash)
{
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Добавление файлов модели ")+_info_file));
    QString path_info=_path+"/"+_info_file;

    CheckFile(_num, path_info);

    CheckInfoFiles(_num, _path);

    QString info_file=_path+"/"+_info_file;
    QString model_structs=info_file.left(info_file.length()-4)+"files";

    CheckFile(_num, model_structs);


    QFileInfo info_list(model_structs);
    _list_date_time=info_list.lastModified();
    tCalcHash ch;
    _list_files_hash=ch.GetFileHash(model_structs);
    QFile list_files(model_structs);

    QSqlQuery files_model(db);

    files_model.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Del, Size)"
                        "VALUES(?, ?, ?, ?, ?, ?, ?);");

    if(!list_files.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << QString::fromUtf8("Не удалось открыть файл списка ") << model_structs;
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Не удалось открыть файл списка ")+model_structs));
        return;
    }

    log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Читаем список файлов модели ")+model_structs));
    tCalcHash calc_summ_hash;
    while(!list_files.atEnd())
    {
        QString fp=list_files.readLine();
        QString file_path=fp;
        QString s=fp.right(1);
        if(s=="\n")
        {
            file_path=fp.left(fp.length()-1);
        }
        qDebug() << QString::fromUtf8(file_path.toAscii());

        QString root=my_settings.GetRoot();

        log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Обрабатываем файл ")+root+file_path));

        QFileInfo info_file(root+file_path);
        if(info_file.exists())
        {
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Файл есть на диске ")+root+file_path));
            tCalcHash calc_file_hash;
            QString file_hash=calc_file_hash.GetFileHash(root+file_path);
            calc_summ_hash.AddToHash(file_hash.toAscii());

            QDateTime file_last_mod=info_file.lastModified();

            files_model.bindValue(0, _num);
            files_model.bindValue(1, NormalizePathFiles(file_path));
            files_model.bindValue(2, file_last_mod);
            files_model.bindValue(3, file_hash);
            files_model.bindValue(4, 1);
            files_model.bindValue(5, 0);
            files_model.bindValue(6, info_file.size());

            if(!files_model.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ при записи файла списка ") << root+file_path;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t +++ ОШИБКА +++ при записи файла списка ")+root+file_path));}

        }
    }


    list_files.close();


    log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Подсчет хеш-суммы файлов списка модели ")+QString::number(_num)));

    //обойти весь список файлов модели в базе в алфавитном порядке поля File(тут уже все файлы модели есть)
    tCalcHash ch_model;
    QSqlQuery summ_hash(db);
    summ_hash.prepare("SELECT Hash, File FROM Files WHERE Model="+QString::number(_num)+" ORDER BY File");
    if(!summ_hash.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ выборки подсчета суммарного хеша модели ") << _num;
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t +++ ОШИБКА +++ выборки подсчета суммарного хеша модели ")+QString::number(_num)));}
    while(summ_hash.next())
    {
        QString hash=summ_hash.value(0).toString();
        QString name=summ_hash.value(1).toString();

        log.Write(QString(QString::fromUtf8("tDatabaseOp \t AddModelFiles \t Обсчет хеш суммы файла ")+name));

         ch_model.AddToHash(hash.toAscii());
    }
    _summ_list_hash=ch_model.ResultHash();
}
//----------------------------------------------------------
QString tDatabaseOp::NormalizePathFiles(QString _path) const
{
    QString before="\\";
    QString after="/";
    QString ret=_path.replace(before, after);
    return ret;
}
//----------------------------------------------------------
void tDatabaseOp::UpdateModelFiles(const qlonglong _num, const QString _path, QString _info_file,  QString &_summ_list_hash)
{
    //Обработать файл модели, файл списка файлов и дополнительные файлы в папке .info

    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Подсчет суммарного хеша файлов модели ")+_info_file));

    //Реализовать добавление новых файлов к существующей модели
    //А также проверку наличия файлов с учетом номера модели
    //Одинаковые Файлы в папке Common должны добавляться для каждой модели отдельно

    //Отметим все файлы модели как ненайденые а потом будем отмечать те что есть на диске и добавлять новые. В конце удалим те что уже нет
    QSqlQuery mark_unfound(db);
    mark_unfound.prepare("UPDATE Files SET Found=0 WHERE Model="+QString::number(_num));
    if(!mark_unfound.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ снятия отметки Found у файлов модели номер ") << _num;
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ снятия отметки Found у файлов модели номер ")+QString::number(_num)));}


    //Файл модели
    QString path_info=_path+"/"+_info_file;
    CheckFile(_num, path_info);

    //Дополнительные графические файлы в папке .info
    CheckInfoFiles(_num, _path);

    //Файл списка файлов модели

    QString full_path=_path+"/"+_info_file;
    QString relat_path=full_path.right(full_path.length()-root.length());
    QString model_structs=full_path.left(full_path.length()-4)+"files";

    CheckFile(_num, model_structs);

    //Обойти весь список файлов в переданом файле
    QFileInfo info_list(model_structs);
    //Проверить наличие списка файлов на диске и его соответствие базе

    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Проверим наличие файлов в базе и соответствие базе ")+model_structs));

    if(info_list.exists())
    {
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файл на диске есть ")+model_structs));

        //список файлов на диске есть
        QFile list_files(model_structs);
        if(!list_files.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << QString::fromUtf8("Не удалось открыть файл списка (обновление) ") << model_structs;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Не удалось открыть файл списка (обновление) ")+model_structs));

            return;
        }

        while(!list_files.atEnd())
        {
            QString fp=NormalizePathFiles(list_files.readLine());
            QString file_path=fp;
            QString s=fp.right(1);
            if(s=="\n")
            {
                file_path=fp.left(fp.length()-1);
            }
            //если есть, проверяем наличие на диске и совпадение времени изменения
            //если отличается - заменяем в базе
            //если файла на диске нет - отметить запиь на удаление

            //если файла нет в базе но есть на диске - вносим по новой.

            //если нет ни на диске ни в базе - ничего не делаем


            log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файл списка файлов ")+file_path));

            //по relat_path проверяем наличие файла в базе
            QSqlQuery search_file(db);
            search_file.prepare("SELECT count(*) FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
            if(!search_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ отпределения наличия файла для обновления ") << file_path;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ отпределения наличия файла для обновления ")+file_path));}

            search_file.next();
            QString hash="";
            int c=search_file.value(0).toInt();
            if(c!=0)
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файл в базе есть ")+file_path));
                //файл в базе есть
                //получим дату-время модификации из базы.
                QSqlQuery dt_file(db);
                dt_file.prepare("SELECT LastMod FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                if(!dt_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения даты-времени модификации файла для обновления ") << relat_path;
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ получения даты-времени модификации файла для обновления ")+relat_path));}

                dt_file.next();
                QDateTime base_dt=dt_file.value(0).toDateTime();

                //проверить наличие файла на диске.
                QFileInfo info(root+file_path);
                if(info.exists())
                {
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файл на диске есть ")+file_path));
                    //Файл на диске есть
                    QDateTime last_mod=info.lastModified();
                    QDateTime rount_dt=RoundDateTime(last_mod);

                    QString file_hash="";
                    if(rount_dt!=base_dt)
                    {
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файлы в базе и на диске не совпадают, берем хеш с диска ")+file_path));
                        //Не совпадает, найти хеш-сумму файла на диске и обновить запись
                        tCalcHash ch;
                        file_hash=ch.GetFileHash(root+file_path);

                        QSqlQuery update_file(db);
                        update_file.prepare("UPDATE Files SET LastMod='"+rount_dt.toString(Qt::ISODate)+"', Hash='"+file_hash+"', Size="+QString::number(info.size())+",Found=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!update_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ обновления даты-времени модификации и хеш-суммы файла ") << relat_path;
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ обновления даты-времени модификации и хеш-суммы файла ")+relat_path));}


                    }
                    else
                    {
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файлы в базе и на диске совпадают, берем хеш с базы ")+file_path));
                        //прочитать из базы хеш-сумму
                        QSqlQuery search_hash(db);
                        search_hash.prepare("SELECT Hash FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!search_hash.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения хеш-суммы файла из базы ") << relat_path;
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ получения хеш-суммы файла из базы ")+relat_path));}

                        search_hash.next();

                        hash=search_hash.value(0).toString();

                        //отметить файл как существующий
                        QSqlQuery check_found_file(db);
                        check_found_file.prepare("UPDATE Files SET Found=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!check_found_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ обновления наличия файла ") << relat_path;
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ обновления наличия файла ")+relat_path));}

                    }
                }
                else
                {
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файла на диске нет ")+file_path));
                    //файла на диске нет
                    //пометить для удаления эту запись
                    QSqlQuery del_check(db);
                    del_check.prepare("UPDATE Files SET Found=0 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                    if(!del_check.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ пометки на удаление ") << file_path;
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ пометки на удаление ")+file_path));}

                }
            }
            else
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файла в базе нет ")+file_path));
                //файла в базе нет
                //проверим наличие файла на диске

                QFileInfo info(root+file_path);

                if(info.exists())
                {
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Файлна диске есть ")+file_path));
                    //Файл на диске есть
                    //Определить дату-время модификации и хэш-сумму и внести файл в базу

                    QDateTime last_mod=info.lastModified();
                    tCalcHash ch;
                    hash=ch.GetFileHash(root+file_path);

                    QSqlQuery insert_file(db);
                    /*
                 *    files_model.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found)"
                             "VALUES(?, ?, ?, ?, ?);");
                             */
                    insert_file.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");
                    insert_file.bindValue(0, _num);
                    insert_file.bindValue(1, file_path);
                    insert_file.bindValue(2, last_mod);
                    insert_file.bindValue(3, hash);
                    insert_file.bindValue(4, 1);
                    insert_file.bindValue(5, info.size());

                    if(!insert_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ добавления нового файла ") << file_path;
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ добавления нового файла ")+file_path));}



                }

            }
//            ch_model.AddToHash(hash.toAscii());
        }
        list_files.close();

//        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Подсчет суммарного хеша модели")+QString::number(_num)));
//        //обойти весь список файлов модели в базе в алфавитном порядке поля File(тут уже все файлы модели есть)
//        tCalcHash ch_model;
//        QSqlQuery summ_hash(db);
//        summ_hash.prepare("SELECT Hash, File FROM Files WHERE Model="+QString::number(_num)+" AND Found=1 ORDER BY File");
//        if(!summ_hash.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ выборки подсчета суммарного хеша модели ") << _num;
//        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t +++ ОШИБКА +++ выборки подсчета суммарного хеша модели ")+QString::number(_num)));}

//        while(summ_hash.next())
//        {
//            QString hash=summ_hash.value(0).toString();
//            QString name=summ_hash.value(1).toString();
//            log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Обработка файла ")+name));
//             ch_model.AddToHash(hash.toAscii());
//        }
//        _summ_list_hash=ch_model.ResultHash();
    }

    //Удалить те файлы модели что не имеют отметки Found
    QSqlQuery delete_unfound(db);
    delete_unfound.prepare("DELETE FROM Files WHERE Model="+QString::number(_num)+" AND Found=0");
    if(!delete_unfound.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления файлов Found=0 модели номер ") << _num;
    log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ удаления файлов Found=0 модели номер ")+QString::number(_num)));}


    tCalcHash ch_model;

    log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Подсчет суммарного хеша модели ")+QString::number(_num)));
    QSqlQuery files_mod(db);
    files_mod.prepare("SELECT Hash, File FROM Files WHERE Model="+QString::number(_num)+" ORDER BY File");
    if(!files_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки файлов модели для подсчета суммарного хеша ") << _num;
    log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ выборки файлов модели для подсчета суммарного хеша ")+QString::number(_num)));}

    while(files_mod.next())
    {
        QString hash=files_mod.value(0).toString();
        QString name=files_mod.value(1).toString();
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t UpdateModelFiles \t Обработка файла ")+name));
        ch_model.AddToHash(hash.toAscii());
    }
_summ_list_hash=ch_model.ResultHash();
}
//----------------------------------------------------------
QDateTime tDatabaseOp::RoundDateTime(const QDateTime &_dt) const
{
    qint64 Msec=_dt.toMSecsSinceEpoch();
    qint64 Sec=Msec/1000;
    qint64 Msec1=Sec*1000;

    QDateTime RoundDT;
    RoundDT.setMSecsSinceEpoch(Msec1);
    return RoundDT;
}
//----------------------------------------------------------
void tDatabaseOp::CheckFile(const qlonglong _num, const QString &_path_file)
{

    QString relat=_path_file.right(_path_file.length()-root.length());

    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Проверка файла ")+relat));

    QFileInfo info(_path_file);
    QSqlQuery mod_select(db);
    mod_select.prepare("SELECT count(*) FROM Files WHERE File='"+relat+"' AND Model="+QString::number(_num));
    if(!mod_select.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ поиска файла модели  ") << relat << _num;
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t +++ ОШИБКА +++ поиска файла модели ")+relat+" "+QString::number(_num)));}

    mod_select.next();
    int s=mod_select.value(0).toInt();
    if(s!=0)
    {
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файл в базе есть ")+relat));
        //файл модели в базе есть

        //Проверить дату=время модификации

        //проверим есть ли файл на диске
        if(info.exists())
        {
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файл есть на диске ")+relat));
            //файл есть на диске
            QSqlQuery select_date(db);
            select_date.prepare("SELECT LastMod FROM Files WHERE File='"+relat+"' AND Model="+QString::number(_num));
            if(!select_date.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ поиска даты-времени модификации файла модели  ") << relat << _num;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t +++ ОШИБКА +++ поиска даты-времени модификации файла модели ")+relat+" "+QString::number(_num)));}
            select_date.next();
            QDateTime db_last_mod=select_date.value(0).toDateTime();
            //дата время файла на диске
            QDateTime disk_last_mod=RoundDateTime(info.lastModified());
            if(db_last_mod==disk_last_mod)
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файл не изменялся ")+relat));
                //файл не изменялся

                //отмечаем файл в базе как существующий
                QSqlQuery mod_check(db);
                mod_check.prepare("UPDATE Files SET Found=1 WHERE File='"+relat+"' AND Model="+QString::number(_num));
                if(!mod_check.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ пометки Found=1 файла модели  ") << relat << _num;
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t +++ ОШИБКА +++ пометки Found=1 файла модели ")+relat+" "+QString::number(_num)));}

            }
            else
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файл изменялсяь ")+relat));
                //файл изменялся

                //обновить дату-время, хэш и отметить как существующий
                tCalcHash ch;
                QString hash=ch.GetFileHash(_path_file);

                QSqlQuery update_dt_hash(db);
                update_dt_hash.prepare("UPDATE Files SET LastMod='"+RoundDateTime(disk_last_mod).toString(Qt::ISODate)+"', Hash='"+hash+"', Size="+QString::number(info.size())+", Found=1 WHERE File='"+relat+"' AND Model="+QString::number(_num));
                if(!update_dt_hash.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ обновления даты-времени и хеша файла модели  ") << relat << _num;
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t +++ ОШИБКА +++ обновления даты-времени и хеша файла модели ")+relat+" "+QString::number(_num)));}

            }

        }
        else
        {
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файла нет на диске ")+relat));
            //файла нет на диске
            //Отметить как не существующий
            QSqlQuery set_unfound(db);
            set_unfound.prepare("UPDATE Files SET Found=0 WHERE File='"+relat+"' AND Model="+QString::number(_num));
            if(!set_unfound.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ отметки как несуществующего файла модели  ") << relat << _num;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t +++ ОШИБКА +++ отметки как несуществующего файла модели ")+relat+" "+QString::number(_num)));}

        }


    }
    else
    {
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файла в базе нет ")+relat));
        //файла модели в базе нет
        //добавим файл модели

        //Проверим есть ли файл на диске

        if(info.exists())
        {
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t Файл есть на диске ")+relat));
            //Файл в базе есть
            //Узнать его параметры
            QDateTime last_mod=info.lastModified();
            tCalcHash ch;
            QString hash=ch.GetFileHash(_path_file);


            //        CH_Model.AddToHash(hash.toAscii());
            QSqlQuery insert_file(db);
            insert_file.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");
            insert_file.bindValue(0, _num);
            insert_file.bindValue(1, relat);
            insert_file.bindValue(2, last_mod);
            insert_file.bindValue(3, hash);
            insert_file.bindValue(4, 1);
            insert_file.bindValue(5, info.size());

            if(!insert_file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ добавления нового файла модели ") << relat << _num;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckFile \t +++ ОШИБКА +++ добавления нового файла модели ")+relat+" "+QString::number(_num)));}

        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::CheckInfoFiles(const qlonglong _num, const QString &_folder_mod)
{
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t Обработка файлов в инфо-папке ")+_folder_mod));

    QString info_folder=_folder_mod;
    QString relat=info_folder.right(info_folder.length()-root.length());
    QDir dir(info_folder);
    QStringList lst_files = dir.entryList(QDir::Files | QDir::Hidden);
    foreach(QString entry, lst_files)
    {
        QFileInfo info(NormalizePathFiles(info_folder+"/"+entry));
        QString suffix=info.suffix().toLower();
        if(suffix=="bmp" | suffix=="jpg" | suffix=="jpeg" | suffix=="gif" | suffix=="png")
        {
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t На диске найден файл ")+relat+"/"+entry));
            //проверяем есть ли такой файл в базе
            QSqlQuery select_info_files(db);
            select_info_files.prepare("SELECT count(*) FROM Files WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
            if(!select_info_files.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ поиска файла в папке .info ") << relat+"/"+entry << _num;
            log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t +++ ОШИБКА +++ поиска файла в папке .info ")+relat+"/"+entry+" "+QString::number(_num)));}

            select_info_files.next();
            int s=select_info_files.value(0).toInt();
            if(s!=0)
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t В базе есть файл ")+relat+"/"+entry));
                //Такой файл в базе есть,

                //проверить есть ли такой файл на диске
//                if(info.exists())
//                {
//                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t На диске найден файл (ВТОРИЧНО?)")+relat+"/"+entry));
                    //такой файл на диске есть, мы его только что нашли
                    QSqlQuery select_date(db);
                    select_date.prepare("SELECT LastMod FROM Files WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                    if(!select_date.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ поиска даты-времени модификации файла в папке .info  ") << relat << _num;
                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t +++ ОШИБКА +++ поиска даты-времени модификации файла в папке .info ")+relat+" "+QString::number(_num)));}

                    select_date.next();
                    QDateTime db_last_mod=select_date.value(0).toDateTime();

                    //проверить совпадает ли дата-время модификации
                    QDateTime disk_last_mod=RoundDateTime(info.lastModified());

                    if(db_last_mod==disk_last_mod)
                    {
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t Файл не изменялся ")+relat+"/"+entry));
                        //Файл не изменялся
                        //отметить его как существующий

                        QSqlQuery mod_check(db);
                        mod_check.prepare("UPDATE Files SET Found=1 WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                        if(!mod_check.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ пометки Found=1 файла в папке .info  ") << relat << _num;
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t +++ ОШИБКА +++ пометки Found=1 файла в папке .info ")+relat+" "+QString::number(_num)));}

                    }
                    else
                    {
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t Файл изменялся ")+relat+"/"+entry));
                        //Файл изменялся
                        //откорректируем дату-время, хэш файла и отметим его как существующий
                        tCalcHash ch;
                        QString hash=ch.GetFileHash(info_folder+"/"+entry);

                        QSqlQuery update_dt_hash(db);

                        update_dt_hash.prepare("UPDATE Files SET LastMod='"+disk_last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, Size="+QString::number(info.size())+" WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                        if(!update_dt_hash.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ обновления даты-времени и хеша файла в папке .info  ") << relat << _num;
                        log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t +++ ОШИБКА +++ обновления даты-времени и хеша файла в папке .info ")+relat+" "+QString::number(_num)));}

                    }
//                }
//                else
//                {
//                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t Такого файла на диске нет. Помоему такого уже быть не может ")+relat+"/"+entry));
//                    //такого файла на диске нет

//                    //пометить как несуществующий
//                    QSqlQuery set_unfound(db);
//                    set_unfound.prepare("UPDATE Files SET Found=0 WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
//                    if(!set_unfound.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ отметки как несуществующего файла в папке .info  ") << relat << _num;
//                    log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t +++ ОШИБКА +++ отметки как несуществующего файла в папке .info ")+relat+" "+QString::number(_num)));}

//                }
            }
            else
            {
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t Файла в базе нет, добавляем ")+relat+"/"+entry));
                //такого файла в базе нет, добавляем
                QDateTime last_mod=info.lastModified();
                tCalcHash ch;
                QString hash=ch.GetFileHash(info_folder+"/"+entry);

                QSqlQuery insert_info_files(db);
                insert_info_files.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");
                insert_info_files.bindValue(0, _num);
                insert_info_files.bindValue(1, relat+"/"+entry);
                insert_info_files.bindValue(2, last_mod);
                insert_info_files.bindValue(3, hash);
                insert_info_files.bindValue(4, 1);
                insert_info_files.bindValue(5, info.size());

                if(!insert_info_files.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ добавления графических инфо-файлов ") << _folder_mod+"/"+entry;
                log.Write(QString(QString::fromUtf8("tDatabaseOp \t CheckInfoFiles \t +++ ОШИБКА +++ добавления графических инфо-файлов ")+_folder_mod+"/"+entry));}

            }
        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::ClearModels()
{
    QSqlQuery delete_unfound_models(db);
    delete_unfound_models.prepare("DELETE FROM StructModels WHERE Found=0");
    if(!delete_unfound_models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ удалении не найденых на диске моделей ");
    log.Write(QString::fromUtf8("tDatabaseOp \t ClearModels \t +++ ОШИБКА +++ удалении не найденых на диске моделей "));}
}
//----------------------------------------------------------
void tDatabaseOp::GetListModels(QDataStream &_out, QString& _login)
{
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t RefreshModelsFiles \t Формирование списка моделей ")));
    db.transaction();
    //Необходимо сформировать и передать структуру данных следующего вида:

    //- Число моделей

    //----- Цикл по моделям
    //{
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

    //- Количество файлов модели

    //----- Цикл по файлам
    //{
    //- File
    //- Size
    //- LastMod
    //- Hash
    //- Серверный номер файла
    //- Номер логина последнего изменявшего файл
    //}

    //}

    //Определим количество моделей (реализуем с учетом доступа)
//    QSqlQuery count_mod(db);
//    count_mod.prepare("SELECT Count(*) FROM StructModels");
//    if(!count_mod.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ определения числа передаваемых моделей ");
//    log.Write(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ определения числа передаваемых моделей "));}

    bool no_delete=IsNoDelete(_login);
    QSqlQuery count_mod(db);
    qlonglong num_login=GetNumLogin(_login);
    if(!no_delete)
    {
        //Для обычного пользователя по разрешениям


    count_mod.prepare("SELECT Count(*) FROM ModelRead WHERE Login="+QString::number(num_login));
    if(!count_mod.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ определения числа передаваемых моделей ");
    log.Write(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ определения числа передаваемых моделей "));}



    }
    else
    {
        //для админа все модели

        count_mod.prepare("SELECT Count(*) FROM StructModels");
        if(!count_mod.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ определения числа передаваемых моделей ");
        log.Write(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ определения числа передаваемых моделей "));}
    }

    count_mod.next();
    int cm=count_mod.value(0).toInt();
    _out << cm;

    QSqlQuery models(db);
    if(!no_delete)
    {

    models.prepare("SELECT DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash, Num FROM StructModels INNER JOIN ModelRead ON Num=Model WHERE Login="+QString::number(num_login));
    if(!models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения данных передаваемых моделей ");
    log.Write(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ получения данных передаваемых моделей "));}
    }
    else
    {

        models.prepare("SELECT DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash, Num FROM StructModels");
        if(!models.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения данных передаваемых моделей ");
        log.Write(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ получения данных передаваемых моделей "));}
    }
//    models.next();
    while(models.next())
    {
        //получение и передача данных модели
        QString DiskFile=models.value(0).toString();
        QString Title=models.value(1).toString();
        QString Description=models.value(2).toString();
        QString Struct=models.value(3).toString();
        QDateTime LastMod=models.value(4).toDateTime();
        QString Hash=models.value(5).toString();
        QDateTime ListFilesLastMod=models.value(6).toDateTime();
        QString ListFilesHash=models.value(7).toString();
        QString SummListHash=models.value(8).toString();
        qlonglong Num_S=models.value(9).toLongLong();

        _out << DiskFile;
        _out << Title;
        _out << Description;
        _out << Struct;
        _out << LastMod;
        _out << Hash;
        _out << ListFilesLastMod;
        _out << ListFilesHash;
        _out << SummListHash;
        _out << Num_S;

        //запрос для получения количества файлов модели по ее номеру
        QSqlQuery count_files(db);
        count_files.prepare("SELECT Count(*) FROM Files WHERE Model="+QString::number(Num_S));
        if(!count_files.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения количества файлов модели ") << Num_S;
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ получения количества файлов модели ")+QString::number(Num_S)));}

        count_files.next();
        int cf=count_files.value(0).toInt();

        _out << cf;

        //Получение списка файлов модели
        QSqlQuery files(db);
        files.prepare("SELECT File, Size, LastMod, Hash, Num, WrittenWho FROM Files WHERE Model="+QString::number(Num_S));
        if(!files.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения списка файлов модели ") << Num_S;
        log.Write(QString(QString::fromUtf8("tDatabaseOp \t GetListModels \t +++ ОШИБКА +++ получения списка файлов модели ")+QString::number(Num_S)));}

//        files.next();
        while(files.next())
        {
            QString File=files.value(0).toString();
            qlonglong Size=files.value(1).toLongLong();
            QDateTime LastMod_F=files.value(2).toDateTime();
            QString Hash_F=files.value(3).toString();
            qlonglong Num_F=files.value(4).toLongLong();
            qlonglong wr_who=files.value(5).toLongLong();

            _out << File;
            _out << Size;
            _out << LastMod_F;
            _out << Hash_F;
            _out << Num_F;
            _out << wr_who;
        }

    }
    db.commit();
}
//----------------------------------------------------------
void tDatabaseOp::GetLocalModelFiles(const QString &_str, QStringList &_list)
{
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t GetLocalModelFiles \t Выборка файлов модели ")));
    db.transaction();
    _list.clear();
    QSqlQuery list_model_structs(db);
    QString s="SELECT Files.File FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num WHERE StructModels.Struct='"+_str+"'";
            list_model_structs.prepare(s);
    if(!list_model_structs.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ выборки файлов модели ") << _str;
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t GetLocalModelFiles \t +++ ОШИБКА +++ выборки файлов модели ")+_str));}

//    list_model_structs.next();
    while(list_model_structs.next())
    {
        QString File=list_model_structs.value(0).toString();
        _list.push_back(File);
    }
    db.commit();
}
//----------------------------------------------------------
QString tDatabaseOp::GetHash(const QString& name_file) const
{
    QSqlQuery file(db);
    file.prepare("SELECT Hash, Count(*) FROM Files WHERE File='"+name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения хеша файла ") << name_file;}
//    log.Write(QString(QString::fromUtf8("tDatabaseOp \t GetHash \t +++ ОШИБКА +++ получения хеша файла ")+name_file));


    file.next();
//    int N=file.value(1).toInt();
    return file.value(0).toString();
}
//----------------------------------------------------------
QDateTime tDatabaseOp::GetLastMod(const QString& name_file) const
{
    QSqlQuery file(db);
    file.prepare("SELECT LastMod, Count(*) FROM Files WHERE File='"+name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ получения даты-времени модификации файла ") << name_file;}
//    log.Write(QString(QString::fromUtf8("tDatabaseOp \t GetLastMod \t +++ ОШИБКА +++ получения даты-времени модификации файла ")+name_file));


    file.next();
//    int N=file.value(1).toInt();
    QDateTime ret=file.value(0).toDateTime();
    return ret;
}
//----------------------------------------------------------
void tDatabaseOp::Update_LastMod_Hash(const QString& _file_name, const QDateTime& _last_mod, const QString& _hash)
{
    QSqlQuery update_lm_hash(db);
    update_lm_hash.prepare("UPDATE Files SET LastMod='"+_last_mod.toString(Qt::ISODate)+"', Hash='"+_hash+"' WHERE File='"+_file_name+"'");
    if(!update_lm_hash.exec()){qDebug() << QString::fromUtf8("+++ ОШИБКА +++ обновления даты-времени модификации файла и хеша ") << _file_name;
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t Update_LastMod_Hash \t +++ ОШИБКА +++ обновления даты-времени модификации файла и хеша ")+_file_name));}

}
//----------------------------------------------------------
QString tDatabaseOp::SaveLoginPass(QString& _login, QString& _pass, bool _new_user, qlonglong &num_log)
{
    //Если _new_user истина то нужно проверить не используется ли уже такой пароль
    //если используется то вернуть ошибку (сообщение)
    //если не используется - внести данные и вернуть пустую строку

    //если _new_user ложь то найти такой логин
    //если такого логина нет то вернуть ошибку (сообщение)
    //если такой логин есть - отредактировать запись и вернуть пустую строку

    QString ret="";

    if(_new_user)
    {
        //создание нового пользователя
        QSqlQuery is_log_present(db);
        is_log_present.prepare("SELECT Count(*) FROM Logins WHERE Login='"+_login+"'");
        if(!is_log_present.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки новизны нового логина ") << _login;
        log.Write(QString(QString("tDatabaseOp \t SaveLoginPass \t ++ ОШИБКА ++ проверки новизны нового логина ")+_login.toUtf8()));}
        is_log_present.next();

        int c=is_log_present.value(0).toInt();
        if(c==0)
        {
            //логин новый
            QSqlQuery insert_new_user(db);
            insert_new_user.prepare("INSERT INTO Logins (Login, PassHash, NoDelete, Writable) VALUES (?, ?, ?, ?)");

            insert_new_user.bindValue(0, _login);
            tCalcHash ch;
            ch.AddToHash(_pass.toAscii());
            insert_new_user.bindValue(1, ch.ResultHash());
            insert_new_user.bindValue(2,0);
            insert_new_user.bindValue(3,0);

            if(!insert_new_user.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления нового логина ") << _login;
            log.Write(QString(QString("tDatabaseOp \t SaveLoginPass \t ++ ОШИБКА ++ добавления нового логина ")+_login.toUtf8()));}
            num_log=insert_new_user.lastInsertId().toLongLong();
        }
        else
        {
            //логин уже используется
            ret="Login "+_login+" не создан.\nПользователь с таким логином уже существует!\nРегистрация невозможна!";
        }
    }
    else
    {
        //редактирование имеющегося пользователя
        QSqlQuery is_log_present(db);
        is_log_present.prepare("SELECT Count(*) FROM Logins WHERE Num="+QString::number(num_log));
        if(!is_log_present.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки новизны нового логина ") << _login;
        log.Write(QString(QString("tDatabaseOp \t SaveLoginPass \t ++ ОШИБКА ++ проверки новизны нового логина ")+_login.toUtf8()));}
        is_log_present.next();

        int c=is_log_present.value(0).toInt();
        if(c==0)
        {
            //такого пользователя нет!
            ret="Login "+_login+"(номер "+QString::number(num_log)+") не отредактирован.\nТакого пользователя нет!\nРедактирование невозможно!";
        }
        else
        {
            //пользователь есть
            tCalcHash ch;
            ch.AddToHash(_pass.toAscii());
            QSqlQuery update_user(db);
            update_user.prepare("UPDATE Logins SET Login='"+_login+"', PassHash='"+ch.ResultHash()+"' WHERE Num="+QString::number(num_log));
            if(!update_user.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления логина ") << _login;
            log.Write(QString(QString("tDatabaseOp \t SaveLoginPass \t ++ ОШИБКА ++ обновления логина ")+_login.toUtf8()));}

        }
    }

    return ret;
}
//----------------------------------------------------------
QString tDatabaseOp::DeleteLogin(qlonglong num_login)
{
    QString ret="";
    log.Write(QString(QString::fromUtf8("tDatabaseOp \t DeleteLogin \t Удаление логина номер ")+QString::number(num_login)));

    //Проверить наличие такого номера
    //если номер существует то удалить запись с этим номером

    QSqlQuery select_delete_row(db);
    select_delete_row.prepare("SELECT Count(*) FROM Logins WHERE Num="+QString::number(num_login));
    if(!select_delete_row.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки наличия логина для удаления ") << num_login;
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ проверки наличия логина для удаления ")+QString::number(num_login)));}

    select_delete_row.next();
    int c=select_delete_row.value(0).toInt();

    if(c!=0)
    {
        //логин найден
        //проверить не админа ли это логин
        QSqlQuery is_admin(db);
        is_admin.prepare("SELECT NoDelete FROM Logins WHERE Num="+QString::number(num_login));
        if(!is_admin.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки наличия логина для удаления ") << num_login;
        log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ проверки наличия логина для удаления ")+QString::number(num_login)));}
        is_admin.next();

        bool no_del=is_admin.value(0).toBool();
        if(no_del)
        {
            ret="Это логин администратора. Его нельзя удалять.";
        }
        else
        {


        QSqlQuery del_login(db);
        del_login.prepare("DELETE FROM Logins WHERE Num="+QString::number(num_login));
        if(!del_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления логина ") << num_login;
        log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ удаления логина ")+QString::number(num_login)));}
        }

    }
    else
    {
        //логин не найден
        ret="Логин под номером "+QString::number(num_login)+" не найден";
    }

    return ret;
}
//----------------------------------------------------------
void tDatabaseOp::SendLoginTable(QDataStream &_out)
{
    QSqlQuery count_logins(db);
    count_logins.prepare("SELECT Count(*) FROM Logins");
    if(!count_logins.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ подсчета числа логинов ");
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ подсчета числа логинов ")));}
    count_logins.next();

    int count=count_logins.value(0).toInt();

    _out << count;

    QSqlQuery select_logins(db);
    select_logins.prepare("SELECT Num, Login, PassHash, NoDelete, Writable FROM Logins ORDER BY Num");
    if(!select_logins.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки таблицы логинов ");
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ выборки таблицы логинов ")));}
    while(select_logins.next())
    {
        qlonglong Num=select_logins.value(0).toLongLong();
        QString Login=select_logins.value(1).toString();
        QString PassHash=select_logins.value(2).toString();
        int NoDelete=select_logins.value(3).toInt();
        int Writable=select_logins.value(4).toInt();

        _out << Num;
        _out << Login;
        _out << PassHash;
        _out << NoDelete;
        _out << Writable;
    }


}
//----------------------------------------------------------
void tDatabaseOp::SendReadPermissions(QDataStream &_out)
{
    QSqlQuery count_perm(db);
    count_perm.prepare("SELECT Count(*) FROM ModelRead");
    if(!count_perm.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ подсчета числа разрешений ");
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ подсчета числа разрешений ")));}
    count_perm.next();

    int count=count_perm.value(0).toInt();

    _out << count;

    QSqlQuery select_perm(db);
    select_perm.prepare("SELECT Login, Model FROM ModelRead");
    if(!select_perm.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки таблицы разрешений ");
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ выборки таблицы разрешений ")));}
    while(select_perm.next())
    {
        qlonglong login=select_perm.value(0).toLongLong();
        qlonglong model=select_perm.value(1).toLongLong();

        _out << login;
        _out << model;

    }


}
//----------------------------------------------------------
bool tDatabaseOp::VerifyAutorization(QString& _login, QString& _password)
{
    QSqlQuery sel_login(db);
    sel_login.prepare("SELECT PassHash FROM Logins WHERE Login='"+_login+"'");
    if(!sel_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки логина для авторизации ") << _login;
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ выборки логина для авторизации ")+_login));}
    sel_login.next();

    QString pass_hash=sel_login.value(0).toString();
    tCalcHash ch;
    ch.AddToHash(_password.toAscii());
    return ch.ResultHash()==pass_hash;
}
//----------------------------------------------------------
void tDatabaseOp::SavePermissions(QByteArray _block)
{

    QDataStream out(&_block, QIODevice::ReadOnly);
    out.device()->seek(0);
    out.device()->seek(8);

    qlonglong num_login=0;
    out >> num_login;

    QSqlQuery del_perm_login(db);
    del_perm_login.prepare("DELETE FROM ModelRead WHERE Login="+QString::number(num_login));
    if(!del_perm_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления разрешений по логину номер ") << num_login;
    log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ удаления разрешений по логину номер ")+num_login));}


    int count=0;
    out >> count;

    for(int i=0; i<count; i++)
    {
        qlonglong num_model=0;
        out >> num_model;
        num_model=num_model;

        QSqlQuery insert_perm_login(db);
        insert_perm_login.prepare("INSERT INTO ModelRead (Login, Model) VALUES(?, ?)");

        insert_perm_login.bindValue(0, num_login);
        insert_perm_login.bindValue(1, num_model);
        if(!insert_perm_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления разрешений по логину номер ") << num_login;
        log.Write(QString(QString("tDatabaseOp \t DeleteLogin \t ++ ОШИБКА ++ добавления разрешений по логину номер ")+num_login));}

    }
}
//----------------------------------------------------------
qlonglong tDatabaseOp::GetNumLogin(QString &_login)
{
    qlonglong num_log;

    QSqlQuery search_login_num(db);
    search_login_num.prepare("SELECT Count(*), Num FROM Logins WHERE Login='"+_login+"'");
    if(!search_login_num.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения номера логина ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ получения номера логина ")));}

    search_login_num.next();

    int c=search_login_num.value(0).toInt();
    if(c!=0)
    {
    num_log=search_login_num.value(1).toLongLong();
    }

    return num_log;
}
//----------------------------------------------------------
bool tDatabaseOp::IsNoDelete(QString& _login)
{
    qlonglong num_log=GetNumLogin(_login);
    QSqlQuery sel_no_del(db);
    sel_no_del.prepare("SELECT NoDelete FROM Logins WHERE Num="+QString::number(num_log));
    if(!sel_no_del.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения признака админа ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ получения признака админа ")));}
    sel_no_del.next();

    return sel_no_del.value(0).toBool();

}
//----------------------------------------------------------
void tDatabaseOp::SaveLoginWritable(int row, bool writ)
{
    QSqlQuery sel_login(db);
    sel_login.prepare("SELECT Num FROM Logins ORDER BY Num, Login");
    if(!sel_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения списка логинов ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginWritable \t ++ ОШИБКА ++ получения списка логинов ")));}
    sel_login.next();
    sel_login.first();
    if(!sel_login.seek(row))
    {
        qDebug() << QString::fromUtf8("++ ОШИБКА ++ перехода в логинах по индексу ") << row;
        log.Write(QString(QString("tDatabaseOp \t SaveLoginWritable \t ++ ОШИБКА ++ перехода в логинах по индексу ")+QString::number(row)));
    }
    qlonglong num_login=sel_login.value(0).toLongLong();

    QSqlQuery upd_writ(db);
    if(writ)
    {
        upd_writ.prepare("UPDATE Logins SET Writable=1 WHERE Num="+QString::number(num_login));
    }
    else
    {
        upd_writ.prepare("UPDATE Logins SET Writable=0 WHERE Num="+QString::number(num_login));
    }
    if(!upd_writ.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ изменения writable у логина номер ") << num_login;
    log.Write(QString(QString("tDatabaseOp \t SaveLoginWritable \t ++ ОШИБКА ++ изменения writable у логина номер ")+QString::number(num_login)));}

}
//----------------------------------------------------------
bool tDatabaseOp::VerifyFile(QString& name_file, QString& server_hash)
{
    QSqlQuery sel_hash_file(db);
    sel_hash_file.prepare("SELECT Hash FROM Files WHERE File='"+name_file+"'");
    if(!sel_hash_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска хэша в базе для файла ") << name_file;
    log.Write(QString(QString("tDatabaseOp \t VerifyFile \t ++ ОШИБКА ++ поиска хэша в базе для файла ")+name_file));}
    sel_hash_file.next();

    QString base_hash=sel_hash_file.value(0).toString();
    if(base_hash=="")
    {
        return true;
    }
    return base_hash==server_hash;
}
//----------------------------------------------------------
void tDatabaseOp::CorrectReadPermission(QString& _trans_model, QString& _login)
{
    qlonglong num_login=GetNumLogin(_login);
    QSqlQuery sel_num_model(db);
    sel_num_model.prepare("SELECT Num FROM StructModels WHERE Struct='"+_trans_model+"'");
    if(!sel_num_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска номера модели по структуре ") << _trans_model;
    log.Write(QString(QString("tDatabaseOp \t CorrectReadPermission \t ++ ОШИБКА ++ поиска номера модели по структуре ")+_trans_model));}
    sel_num_model.next();

    qlonglong num_model=sel_num_model.value(0).toLongLong();

    //корректировка разрешений
    //если разрешения нет - добавить
    //если есть - ничего делать не нужно

    QSqlQuery ver_perm(db);
    ver_perm.prepare("SELECT Count(*) FROM ModelRead WHERE Login="+QString::number(num_login)+" AND Model="+QString::number(num_model));
    if(!ver_perm.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ определения наличия разрешения ");
    log.Write(QString(QString("tDatabaseOp \t CorrectReadPermission \t ++ ОШИБКА ++ определения наличия разрешения ")));}
    ver_perm.next();

    int count=ver_perm.value(0).toInt();
    if(count==0)
    {
        QSqlQuery insert_perm(db);
        insert_perm.prepare("INSERT INTO ModelRead (Login, Model) VALUES (?, ?)");
        insert_perm.bindValue(0, num_login);
        insert_perm.bindValue(1, num_model);
        if(!insert_perm.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления разрешения ");
        log.Write(QString(QString("tDatabaseOp \t CorrectReadPermission \t ++ ОШИБКА ++ добавления разрешения ")));}


    }
}
//----------------------------------------------------------
void tDatabaseOp::CorrectWrittenWho(QList<tFileList> list, QString& login)
{
    qlonglong num_login=GetNumLogin(login);
    for(int i=0; i<list.size(); i++)
    {
        QString file_name=list[i].file_name;
        QSqlQuery correct_wr(db);
        correct_wr.prepare("UPDATE Files SET WrittenWho="+QString::number(num_login)+" WHERE File='"+file_name+"'");
        if(!correct_wr.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ редактирование логина записавшего файл ");
        log.Write(QString(QString("tDatabaseOp \t CorrectWrittenWho \t ++ ОШИБКА ++ редактирование логина записавшего файл ")));}

    }
}
