#include "tDatabaseOp.h"
#include "tSettings.h"
#include <QMessageBox>
#include "tCalcHash.h"

extern tSettings my_settings;
//----------------------------------------------------------------------
tDatabaseOp::tDatabaseOp()
{
    QTextCodec *codec =QTextCodec::codecForName("Windows-1251");

    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    //codec->toUnicode
    log.Write(codec->toUnicode("tDatabaseOp \tDatabaseOp\tПодключение базы данных  "));

    root=my_settings.GetRoot();
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Database.db");
    if(!db.open()){qDebug() << db.lastError().text();
        log.Write(QString(codec->toUnicode("tDatabaseOp \tDatabaseOp\t++ ОШИБКА ++ открытия базы данных ")+db.lastError().text()));}
}
//----------------------------------------------------------------------
tDatabaseOp::~tDatabaseOp()
{
    l="tDatabaseOp \tDatabaseOp\tОтключение базы данных  ";
    log.Write(l);
    db.close();
}
//----------------------------------------------------------------------
void tDatabaseOp::RefreshModelsFiles()
{
    l="tDatabaseOp \tRefreshModelsFiles\tОбновление локальных таблиц по файлам на диске  ";
    log.Write(l);

    QString root=my_settings.GetRoot();

    //Начало работы с базой данных
    //Начало транзакции
    bool b=db.transaction();

    QSqlQuery unset_found_models(db);
    unset_found_models.prepare("UPDATE StructModels SET Found=0");

    if(!unset_found_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при сбросе Found моделей ");
        log.Write(QString("tDatabaseOp \t RefreshModelsFiles \t ++ ОШИБКА ++ при сбросе Found моделей "));}

    QSqlQuery unset_found_files(db);
    unset_found_files.prepare("UPDATE Files SET Found=0, Del=0");

    if(!unset_found_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при сбросе Found и Del файлов ");
        log.Write(QString("tDatabaseOp \t RefreshModelsFiles \t ++ ОШИБКА ++ при сбросе Found и Del файлов "));}

    SearchInfo(root);

    //После обработки всех файлов и всей базы можно удалить из базы более не нужные модели у которых Found=0 (они не найдены на диске)
    QSqlQuery delete_unfound_models(db);
    delete_unfound_models.prepare("DELETE FROM StructModels WHERE Found=0");
    if(!delete_unfound_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удалении не найденых на диске моделей ");
        log.Write(QString("tDatabaseOp \t RefreshModelsFiles \t ++ ОШИБКА ++ удалении не найденых на диске моделей "));}

    QSqlQuery delete_unfound_files(db);
    delete_unfound_files.prepare("DELETE FROM Files WHERE Found=0 OR Del=1");
    if(!delete_unfound_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удалении не найденых на диске файлов ");
        log.Write(QString("tDatabaseOp \t RefreshModelsFiles \t ++ ОШИБКА ++ удалении не найденых на диске файлов "));}

    //Окончание работы с базой данных
    //Коммит изменений
    b=db.commit();

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
            //проверяем есть ли такое в базе
            QString relat=_folder.right(_folder.length()-root.length());
            QSqlQuery search_file(db);
            QString S="SELECT count(*) FROM StructModels WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
            search_file.prepare(S);

            if(!search_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при поиске ") << NormalizePathFiles(relat+"/"+entry);
                log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при поиске ") + NormalizePathFiles(relat.toUtf8()+"/"+entry.toUtf8())));}

            search_file.next();

            QFileInfo info(_folder+"/"+entry);
            QDateTime last_modified=info.lastModified();


            if(search_file.value(0).toInt()==0)
            {
                //Такой модели в базе нет
                //заносим найденую модель в базу
                tCalcHash ch;

                QString hash=ch.GetFileHash(_folder+"/"+entry);

                QSettings s(_folder+"/"+entry, QSettings::IniFormat);
                QTextCodec *codec =QTextCodec::codecForName("Windows-1251");
                s.setIniCodec(codec);

                QString title=s.value("Title","").toString();
                QString desc=s.value("Description","").toString();
                QString struct_mod=s.value("Struct","").toString();

                QString file=relat+"/"+entry;
                struct_models.bindValue(0, file);
                struct_models.bindValue(1, title);
                struct_models.bindValue(2, desc);

                struct_models.bindValue(3, struct_mod+"/"+title);
                struct_models.bindValue(4, last_modified);
                struct_models.bindValue(5, hash);
                struct_models.bindValue(6, 1);//Found

                QDateTime list_files_date_time;
                QString list_files_hash="";
                QString summ_list_hash="";

                if(!struct_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при добавлении данных ") << relat+"/"+entry;
                    log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при добавлении данных ") + relat.toUtf8()+"/"+entry.toUtf8()));}

                //добавление списка файлов модели
                //передаем значение автоинкрементного ключа

                AddModelFiles(struct_models.lastInsertId().toLongLong(), _folder, entry, list_files_date_time, list_files_hash, summ_list_hash);

                //Добавляем дату-время файла модели, хеш сумму самого файла модели и сумму сумм файлов в ней описаных
                QSqlQuery update_struct_models(db);

                QString dt=list_files_date_time.toString(Qt::ISODate);

                QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+list_files_hash+"', SummListHash='"+summ_list_hash+"' WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                update_struct_models.prepare(sql);

                if(!update_struct_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при обновлении новых данных ") << relat+"/"+entry;
                    log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при обновлении новых данных ") + relat.toUtf8()+"/"+entry.toUtf8()));}
            }
            else
            {
                //Такая модель в базе есть

                //Необходимо проверить а есть ли такая модель на диске
                QDateTime round_dt;
                QFileInfo info_mod(_folder+"/"+entry);
                if(info_mod.exists())
                {
                    //модель на диске есть
                    //проверим время редактирования


                    QString S="SELECT lastMod, Hash, Num FROM StructModels WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                    search_file.prepare(S);

                    if(!search_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при поиске ") << NormalizePathFiles(relat+"/"+entry);
                        log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при поиске ") + NormalizePathFiles(relat.toUtf8()+"/"+entry.toUtf8())));}

                    search_file.next();

                    QDateTime base_date_time=search_file.value(0).toDateTime();
                    QString list_files_hash=search_file.value(1).toString();
                    qlonglong num_model=search_file.value(2).toLongLong();

                    round_dt=RoundDateTime(last_modified);

                    if(base_date_time!=round_dt)
                    {
                        //читаем Title модели, описание и структуру

                        QSettings s(_folder+"/"+entry, QSettings::IniFormat);
                        QTextCodec *codec =QTextCodec::codecForName("Windows-1251");
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

                        if(!search_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при редактировании ") << NormalizePathFiles(relat+"/"+entry);
                            log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при редактировании ") + NormalizePathFiles(relat.toUtf8()+"/"+entry.toUtf8())));}

                    }
                    //Если такая модель в базе есть то независимо от того менялся ли список файлов
                    //нужно проверить их наличие и не изменились ли они

                    //Проверить список файлов и если есть различия - откорректировать базу
                    //Эти таблицы только для ускорения работы что бы не считать хэш каждый раз

                    QDateTime list_files_date_time=round_dt;

                    QString summ_list_hash="";

                    UpdateModelFiles(num_model, _folder, entry, summ_list_hash);

                    //обновить запись в таблице моделей по дате модификации, хеш-сумме и общей хеш-сумме

                    QSqlQuery update_struct_models(db);

                    QString dt=list_files_date_time.toString(Qt::ISODate);

                    QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+list_files_hash+"', SummListHash='"+summ_list_hash+"', Found=1 WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                    update_struct_models.prepare(sql);

                    if(!update_struct_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при обновлении имеющихся данных ") << relat+"/"+entry;
                        log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при обновлении имеющихся данных ") + relat.toUtf8()+"/"+entry.toUtf8()));}

                }
                else
                {
                    //модели на диске уже нет
                    //поставить пометку Found=false
                    QSqlQuery update_found_false(db);
                    update_found_false.prepare("UPDATE StructModels SET Found=0 Where DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'");
                    if(!update_found_false.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при пометки на удаление модели ") << NormalizePathFiles(relat+"/"+entry);
                        log.Write(QString(QString("tDatabaseOp \t SearchInfo \t ++ ОШИБКА ++ при пометки на удаление модели ")+ NormalizePathFiles(relat.toUtf8()+"/"+entry.toUtf8())));}

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
void tDatabaseOp::AddModelFiles(const qlonglong _num, const QString _path, const QString _info_file, QDateTime &_list_date_time, QString &_list_files_hash, QString &_summ_list_hash)
{
    QString path_info=_path+"/"+_info_file;
    CheckFile(_num, path_info);

    CheckInfoFiles(_num, _path);

    QString info_file=_path+"/"+_info_file;
    QString model_files=info_file.left(info_file.length()-4)+"files";
    CheckFile(_num, model_files);


    QFileInfo info_list(model_files);
    _list_date_time=info_list.lastModified();
    tCalcHash ch;
    _list_files_hash=ch.GetFileHash(model_files);
    QFile list_files(model_files);

    QSqlQuery files_model(db);

    files_model.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Del, Size)"
                        "VALUES(?, ?, ?, ?, ?, ?, ?);");

    if(!list_files.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл списка " << model_files;
        log.Write(QString(QString("tDatabaseOp \t AddModelFiles \t Не удалось открыть файл списка ") + model_files.toUtf8()));
        return;
    }



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
        qDebug() << file_path;

        QString root=my_settings.GetRoot();

        QFileInfo info_file(root+file_path);
        if(info_file.exists())
        {
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

            if(!files_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ при записи файла списка ") << root+file_path;
                log.Write(QString(QString("tDatabaseOp \t AddModelFiles \t ++ ОШИБКА ++ при записи файла списка ") + root+file_path.toUtf8()));}

        }
    }


    list_files.close();
    _summ_list_hash=calc_summ_hash.ResultHash();
}
//----------------------------------------------------------
QString tDatabaseOp::NormalizePathFiles(QString _path) const
{
    QTextCodec *codec =QTextCodec::codecForName("Windows-1251");
    //        QTextCodec *codec =QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);

    QString before="\\";
    QString after="/";
    QString ret=codec->toUnicode(_path.replace(before, after).toAscii());
    return ret;
}
//----------------------------------------------------------
void tDatabaseOp::UpdateModelFiles(const qlonglong _num, const QString _path, const QString _info_file,  QString &_summ_list_hash)
{
    //Обработать файл модели, файл списка файлов и дополнительные файлы в папке .info


    //Реализовать добавление новых файлов к существующей модели
    //А также проверку наличия файлов с учетом номера модели
    //Одинаковые Файлы в папке Common должны добавляться для каждой модели отдельно


    //Файл модели
    QString path_info=_path+"/"+_info_file;

    CheckFile(_num, path_info);

    //Дополнительные графические файлы в папке .info
    CheckInfoFiles(_num, _path);

    //Файл списка файлов модели

    QString full_path=_path+"/"+_info_file;

    QString relat_path=full_path.right(full_path.length()-root.length());
    QString model_files=full_path.left(full_path.length()-4)+"files";

    CheckFile(_num, model_files);

    //Обойти весь список файлов в переданом файле
    QFileInfo info_list(model_files);
    //Проверить наличие списка файлов на диске и его соответствие базе

    if(info_list.exists())
    {
        //список файлов на диске есть
        QFile list_files(model_files);
        if(!list_files.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Не удалось открыть файл списка (обновление) " << model_files;
            log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t Не удалось открыть файл списка (обновление) ") + model_files.toUtf8()));
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




            //по relat_path проверяем наличие файла в базе
            QSqlQuery search_file(db);
            search_file.prepare("SELECT count(*) FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
            if(!search_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отпределения наличия файла для обновления ") << file_path;
                log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ отпределения наличия файла для обновления ") + file_path.toUtf8()));}

            search_file.next();
            QString hash="";
            int c=search_file.value(0).toInt();
            if(c!=0)
            {
                //файл в базе есть
                //получим дату-время модификации из базы.
                QSqlQuery dt_file(db);
                dt_file.prepare("SELECT LastMod FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                if(!dt_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения даты-времени модификации файла для обновления ") << relat_path;
                    log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ получения даты-времени модификации файла для обновления ") + relat_path.toUtf8()));}

                dt_file.next();
                QDateTime base_dt=dt_file.value(0).toDateTime();

                //проверить наличие файла на диске.
                QFileInfo info(root+file_path);
                if(info.exists())
                {
                    //Файл на диске есть
                    QDateTime last_mod=info.lastModified();
                    QDateTime rount_dt=RoundDateTime(last_mod);

                    QString file_hash="";
                    if(rount_dt!=base_dt)
                    {
                        //Не совпадает, найти хеш-сумму файла на диске и обновить запись
                        tCalcHash ch;
                        file_hash=ch.GetFileHash(root+file_path);

                        QSqlQuery update_file(db);
                        update_file.prepare("UPDATE Files SET LastMod='"+rount_dt.toString(Qt::ISODate)+"', Hash='"+file_hash+"', Size="+QString::number(info.size())+",Found=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!update_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления даты-времени модификации и хеш-суммы файла ") << relat_path;
                            log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ обновления даты-времени модификации и хеш-суммы файла ") + relat_path.toUtf8()));}

                    }
                    else
                    {
                        //прочитать из базы хеш-сумму
                        QSqlQuery search_hash(db);
                        search_hash.prepare("SELECT Hash FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!search_hash.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения хеш-суммы файла из базы ") << relat_path;
                            log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ получения хеш-суммы файла из базы ") + relat_path.toUtf8()));}

                        search_hash.next();

                        hash=search_hash.value(0).toString();

                        //отметить файл как существующий
                        QSqlQuery check_found_file(db);
                        check_found_file.prepare("UPDATE Files SET Found=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!check_found_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления наличия файла ") << relat_path;
                            log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ обновления наличия файла ") + relat_path.toUtf8()));}

                    }
                }
                else
                {
                    //файла на диске нет
                    //пометить для удаления эту запись
                    QSqlQuery del_check(db);
                    del_check.prepare("UPDATE Files SET Del=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                    if(!del_check.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ пометки на удаление ") << file_path;
                        log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ пометки на удаление ") + file_path.toUtf8()));}
                }
            }
            else
            {
                //файла в базе нет
                //проверим наличие файла на диске

                QFileInfo info(root+file_path);

                if(info.exists())
                {
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

                    if(!insert_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления нового файла ") << file_path;
                        log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ добавления нового файла ") + file_path.toUtf8()));}


                }
            }
        }
        list_files.close();
    }


    tCalcHash ch_model;


    QSqlQuery files_mod(db);
    files_mod.prepare("SELECT Hash FROM Files WHERE Model="+QString::number(_num)+" ORDER BY File");
    if(!files_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки файлов модели для подсчета суммарного хеша ") << _num;
        log.Write(QString(QString("tDatabaseOp \t UpdateModelFiles \t ++ ОШИБКА ++ выборки файлов модели для подсчета суммарного хеша ")+QString::number(_num)));}

    while(files_mod.next())
    {
        QString hash=files_mod.value(0).toString();
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

    QFileInfo info(_path_file);
    QSqlQuery mod_select(db);
    mod_select.prepare("SELECT count(*) FROM Files WHERE File='"+relat+"' AND Model="+QString::number(_num));
    if(!mod_select.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска файла модели  ") << relat << _num;
        log.Write(QString(QString("tDatabaseOp \t CheckFile \t ++ ОШИБКА ++ поиска файла модели  ") + relat.toUtf8()+ " " +QString::number( _num)));}

    mod_select.next();
    int s=mod_select.value(0).toInt();
    if(s!=0)
    {
        //файл модели в базе есть

        //Проверить дату=время модификации

        //проверим есть ли файл на диске
        if(info.exists())
        {
            //файл есть на диске
            QSqlQuery select_date(db);
            select_date.prepare("SELECT LastMod FROM Files WHERE File='"+relat+"' AND Model="+QString::number(_num));
            if(!select_date.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска даты-времени модификации файла модели  ") << relat << _num;
                log.Write(QString(QString("tDatabaseOp \t CheckFile \t ++ ОШИБКА ++ поиска даты-времени модификации файла модели  ") + relat.toUtf8() +" "+QString::number( _num)));}

            select_date.next();
            QDateTime db_last_mod=select_date.value(0).toDateTime();
            //дата время файла на диске
            QDateTime disk_last_mod=RoundDateTime(info.lastModified());
            if(db_last_mod==disk_last_mod)
            {
                //файл не изменялся

                //отмечаем файл в базе как существующий
                QSqlQuery mod_check(db);
                mod_check.prepare("UPDATE Files SET Found=1 WHERE File='"+relat+"' AND Model="+QString::number(_num));
                if(!mod_check.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ пометки Found=1 файла модели  ") << relat << _num;
                    log.Write(QString(QString("tDatabaseOp \t CheckFile \t ++ ОШИБКА ++ пометки Found=1 файла модели  ") + relat.toUtf8() +" "+QString::number( _num)));}
            }
            else
            {
                //файл изменялся

                //обновить дату-время, хэш и отметить как существующий
                tCalcHash ch;
                QString hash=ch.GetFileHash(_path_file);

                QSqlQuery update_dt_hash(db);
                update_dt_hash.prepare("UPDATE Files SET LastMod='"+RoundDateTime(disk_last_mod).toString(Qt::ISODate)+"', Hash='"+hash+"', Size="+QString::number(info.size())+", Found=1 WHERE File='"+relat+"' AND Model="+QString::number(_num));
                if(!update_dt_hash.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления даты-времени и хеша файла модели  ") << relat << _num;
                    log.Write(QString(QString("tDatabaseOp \t CheckFile \t ++ ОШИБКА ++ обновления даты-времени и хеша файла модели  ") + relat.toUtf8() +" "+QString::number( _num)));}
            }

        }
        else
        {
            //файла нет на диске
            //Отметить как не существующий
            QSqlQuery set_unfound(db);
            set_unfound.prepare("UPDATE Files SET Found=0 WHERE File='"+relat+"' AND Model="+QString::number(_num));
            if(!set_unfound.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отметки как несуществующего файла модели  ") << relat << _num;
                log.Write(QString(QString("tDatabaseOp \t CheckFile \t ++ ОШИБКА ++ отметки как несуществующего файла модели  ") + relat.toUtf8() +" "+QString::number( _num)));}
        }


    }
    else
    {
        //файла модели в базе нет
        //добавим файл модели

        //Проверим есть ли файл на диске

        if(info.exists())
        {
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

            if(!insert_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления нового файла модели ") << relat << _num;
                log.Write(QString(QString("tDatabaseOp \t CheckFile \t ++ ОШИБКА ++ добавления нового файла модели ") + relat.toUtf8() +" "+QString::number( _num)));}
        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::CheckInfoFiles(const qlonglong _num, const QString &_folder_mod)
{
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
            //проверяем есть ли такой файл в базе
            QSqlQuery select_info_files(db);
            select_info_files.prepare("SELECT count(*) FROM Files WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
            if(!select_info_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска файла в папке .info ") << relat+"/"+entry << _num;
                log.Write(QString(QString("tDatabaseOp \t CheckInfoFiles \t ++ ОШИБКА ++ поиска файла в папке .info ") + relat.toUtf8()+"/"+entry.toUtf8() +QString::number( _num)));}

            select_info_files.next();
            int s=select_info_files.value(0).toInt();
            if(s!=0)
            {
                //Такой файл в базе есть,

                //проверить есть ли такой файл на диске
                if(info.exists())
                {
                    //такой файл на диске есть
                    QSqlQuery select_date(db);
                    select_date.prepare("SELECT LastMod FROM Files WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                    if(!select_date.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска даты-времени модификации файла в папке .info  ") << relat << _num;
                        log.Write(QString(QString("tDatabaseOp \t CheckInfoFiles \t ++ ОШИБКА ++ поиска даты-времени модификации файла в папке .info  ") + relat.toUtf8()+QString::number( _num)));}

                    select_date.next();
                    QDateTime db_last_mod=select_date.value(0).toDateTime();

                    //проверить совпадает ли дата-время модификации
                    QDateTime disk_last_mod=RoundDateTime(info.lastModified());

                    if(db_last_mod==disk_last_mod)
                    {
                        //Файл не изменялся
                        //отметить его как существующий

                        QSqlQuery mod_check(db);
                        mod_check.prepare("UPDATE Files SET Found=1 WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                        if(!mod_check.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ пометки Found=1 файла в папке .info  ") << relat << _num;
                            log.Write(QString(QString("tDatabaseOp \t CheckInfoFiles \t ++ ОШИБКА ++ пометки Found=1 файла в папке .info  ") + relat.toUtf8()+QString::number( _num)));}

                    }
                    else
                    {
                        //Файл изменялся
                        //откорректируем дату-время, хэш файла и отметим его как существующий
                        tCalcHash ch;
                        QString hash=ch.GetFileHash(info_folder+"/"+entry);

                        QSqlQuery update_dt_hash(db);

                        update_dt_hash.prepare("UPDATE Files SET LastMod='"+disk_last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, Size="+QString::number(info.size())+" WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                        if(!update_dt_hash.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления даты-времени и хеша файла в папке .info  ") << relat << _num;
                            log.Write(QString(QString("tDatabaseOp \t CheckInfoFiles \t ++ ОШИБКА ++ обновления даты-времени и хеша файла в папке .info  ") + relat.toUtf8()+QString::number( _num)));}

                    }
                }
                else
                {
                    //такого файла на диске нет

                    //пометить как несуществующий
                    QSqlQuery set_unfound(db);
                    set_unfound.prepare("UPDATE Files SET Found=0 WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                    if(!set_unfound.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отметки как несуществующего файла в папке .info  ") << relat << _num;
                        log.Write(QString(QString("tDatabaseOp \t CheckInfoFiles \t ++ ОШИБКА ++ отметки как несуществующего файла в папке .info  ") + relat.toUtf8()+QString::number( _num)));}

                }
            }
            else
            {
                //такого файла в базе нет, добавляем
                QDateTime last_mod=info.lastModified();
                tCalcHash ch;
                QString hash=ch.GetFileHash(info_folder+"/"+entry);

                QSqlQuery insert_info_files(db);
                insert_info_files.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");
                insert_info_files.bindValue(0, _num);
                QString file=relat+"/"+entry;
                insert_info_files.bindValue(1, file);
                insert_info_files.bindValue(2, last_mod);
                insert_info_files.bindValue(3, hash);
                insert_info_files.bindValue(4, 1);
                insert_info_files.bindValue(5, info.size());

                if(!insert_info_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления графических инфо-файлов ") << _folder_mod+"/"+entry;
                    log.Write(QString(QString("tDatabaseOp \t CheckInfoFiles \t ++ ОШИБКА ++ добавления графических инфо-файлов ") + relat.toUtf8()+"/"+entry.toUtf8()));}

            }
        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::ClearModels()
{
    QSqlQuery delete_unfound_models(db);
    delete_unfound_models.prepare("DELETE FROM StructModels WHERE Found=0");
    if(!delete_unfound_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удалении не найденых на диске моделей ");
        log.Write(QString(QString("tDatabaseOp \t ClearModels \t ++ ОШИБКА ++ удалении не найденых на диске моделей ")));}

}
//----------------------------------------------------------
void tDatabaseOp::GetListModels(QStringList &_list)
{
    _list.clear();
    QSqlQuery list_struct_models(db);
    list_struct_models.prepare("SELECT Struct FROM StructModels");
    if(!list_struct_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки структуры моделей ");
        log.Write(QString(QString("tDatabaseOp \t GetListModels \t ++ ОШИБКА ++ выборки структуры моделей ")));}

    //    list_struct_models.next();
    while(list_struct_models.next())
    {
        QString struct_mod=list_struct_models.value(0).toString();
        _list.push_back(struct_mod);
    }
}
//----------------------------------------------------------
void tDatabaseOp::GetLocalModelFiles(QString &_str, QStringList &_list)
{
    _list.clear();
    QSqlQuery list_model_files(db);
    QString s="SELECT Files.File FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num WHERE StructModels.Struct='"+_str+"'";
    list_model_files.prepare(s);
    if(!list_model_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки файлов модели ") << _str;
        log.Write(QString(QString("tDatabaseOp \t GetLocalModelFiles \t ++ ОШИБКА ++ выборки файлов модели ")+_str.toUtf8()));}

    //    list_model_files.next();
    while(list_model_files.next())
    {
        QString File=list_model_files.value(0).toString();
        _list.push_back(File);
    }
}
//----------------------------------------------------------
void tDatabaseOp::SaveServerModelFiles(QByteArray &_block)
{
    l="tDatabaseOp \tSaveServerModelFiles\tОбновление серверных таблиц по информации с сервера  ";
    log.Write(l);

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
    //}

    //}

    QDataStream out(&_block, QIODevice::ReadOnly);

    out.device()->seek(4);
    int num_models=-1;
    out >> num_models;

    db.transaction();

    for(int i=0; i<num_models; i++)
    {
        QString DiskFile="";
        QString Title="";
        QString Description="";
        QString Struct="";
        QDateTime LastMod;
        QString Hash="";
        QDateTime ListFilesLastMod;
        QString ListFilesHash="";
        QString SummListHash="";
        qlonglong Num=0;

        out >> DiskFile;
        out >> Title;
        out >> Description;
        out >> Struct;
        out >> LastMod;
        out >> Hash;
        out >> ListFilesLastMod;
        out >> ListFilesHash;
        out >> SummListHash;
        out >> Num;

        QSqlQuery is_found_mod(db);
        is_found_mod.prepare("SELECT Count(*), Num FROM ServerStructModels WHERE DiskFile='"+DiskFile+"'");
        if(!is_found_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки наличия серверной модели ") << DiskFile;
            log.Write(QString(QString("tDatabaseOp \t SaveServerModelFiles \t ++ ОШИБКА ++ проверки наличия серверной модели  ")+DiskFile));}
        is_found_mod.next();
        int c=is_found_mod.value(0).toInt();

        qlonglong num_server_model=0;
        if(c=0)
        {
            QSqlQuery insert_server_model(db);
            insert_server_model.prepare("INSERT INTO ServerStructModels (DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash, ServerNum) "
                                        "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
            insert_server_model.bindValue(0, DiskFile);
            insert_server_model.bindValue(1, Title);
            insert_server_model.bindValue(2,Description );
            insert_server_model.bindValue(3, Struct);
            insert_server_model.bindValue(4, LastMod);
            insert_server_model.bindValue(5, Hash);
            insert_server_model.bindValue(6, ListFilesLastMod);
            insert_server_model.bindValue(7, ListFilesHash);
            insert_server_model.bindValue(8, SummListHash);
            insert_server_model.bindValue(9, Num);

            if(!insert_server_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления серверной модели ") << DiskFile;
                log.Write(QString(QString("tDatabaseOp \t SaveServerModelFiles \t ++ ОШИБКА ++ добавления серверной модели ")+DiskFile.toUtf8()));}

            num_server_model=insert_server_model.lastInsertId().toLongLong();
        }
        else
        {
            QSqlQuery update_s_mod(db);
            update_s_mod.prepare("UPDATE ServerStructModels SET Title='"+Title+"', Description='"+Description+"', Struct='"+Struct+"', LastMod='"+LastMod.toString(Qt::ISODate)+"', Hash='"+Hash+"', ListFilesLastMod='"+ListFilesLastMod.toString(Qt::ISODate)+"', ListFilesHash='"+ListFilesHash+"', SummListHash='"+SummListHash+"', ServerNum="+QString::number(Num)+"  WHERE DiskFile='"+DiskFile+"'");
            if(!update_s_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления серверной модели ") << DiskFile;
                log.Write(QString(QString("tDatabaseOp \t SaveServerModelFiles \t ++ ОШИБКА ++ обновления серверной модели  ")+DiskFile));}

            num_server_model=is_found_mod.value(1).toInt();

            QSqlQuery del_model_files(db);
            del_model_files.prepare("DELETE FROM ServerFiles WHERE Model="+QString::number(num_server_model));
            if(!del_model_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ очистки файлов серверной модели ") << num_server_model;
                log.Write(QString(QString("tDatabaseOp \t SaveServerModelFiles \t ++ ОШИБКА ++ очистки файлов серверной модели ")+QString::number(num_server_model)));}


        }

        int num_files=-1;

        out >> num_files;

        for(int j=0;j<num_files; j++)
        {
            QString File="";
            qlonglong Size=0;
            QDateTime LastMod_F;
            QString Hash_F="";
            qlonglong Num_F=0;

            out >> File;
            out >> Size;
            out >> LastMod_F;
            out >> Hash_F;
            out >> Num_F;

            QSqlQuery insert_server_files(db);
            insert_server_files.prepare("INSERT INTO ServerFiles (Model, File, Size, lastMod, Hash, ServerNum) VALUES (?, ?, ?, ?, ?, ?);");

            insert_server_files.bindValue(0, num_server_model);
            insert_server_files.bindValue(1, File);
            insert_server_files.bindValue(2, Size);
            insert_server_files.bindValue(3, LastMod_F);
            insert_server_files.bindValue(4, Hash_F);
            insert_server_files.bindValue(5, Num_F);
            if(!insert_server_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления файлов серверной модели ") << num_server_model;
                log.Write(QString(QString("tDatabaseOp \t SaveServerModelFiles \t ++ ОШИБКА ++ добавления файлов серверной модели ")+QString::number(num_server_model)));}

        }
    }



    db.commit();
}
//----------------------------------------------------------
void tDatabaseOp::GetServerListModels(QStringList &_list)
{
    QSqlQuery models(db);
    QString s="SELECT Struct FROM ServerStructModels";
    models.prepare(s);
    if(!models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ составления списка серверных моделей ");
        log.Write(QString("tDatabaseOp \t GetServerListModels \t ++ ОШИБКА ++ составления списка серверных моделей "));}

    while(models.next())
    {
        QString str=models.value(0).toString();
        _list.push_back(str);
    }
}
//----------------------------------------------------------
void tDatabaseOp::GetServerListFiles(const QString &_str, QStringList &_list)
{
    QSqlQuery models(db);
    QString s="SELECT ServerFiles.File FROM ServerStructModels INNER JOIN ServerFiles ON ServerFiles.Model=ServerStructModels.Num WHERE ServerStructModels.Struct='"+_str+"'";
    models.prepare(s);
    if(!models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ составления списка серверных файлов ");
        log.Write(QString("tDatabaseOp \t GetServerListFiles \t ++ ОШИБКА ++ составления списка серверных файлов "));}

    while(models.next())
    {
        QString str=models.value(0).toString();
        _list.push_back(str);
    }

}
//----------------------------------------------------------
QString tDatabaseOp::GetLocalHash(const QString& _name_file, bool& _sending, bool &_receiving)
{
    //Файл есть в базе и на диске - нужно сравнивать даты и хэши
    //Файл есть в базе но нет на диске - при чтении с сервера или удалении с локального - допустимо, нужен хеш с базы
    //файла нет в базе но есть на диске - при записи на сервер допустимо, нужен хеш с диска
    //файла нет ни в базе ни на диске - нормально, функция не должна сбивать работу (как-то)

    //Нужно сравнить дату-время модификации файла в базе и на диске
    //если не совпадают - передать сигнал о том что дело не чисто, локальный файл не совпадает с базой
    //нужно прервать работу с моделью

    QString hash="";
    QDateTime base_date_time;
    QDateTime disk_date_time;
    QFileInfo info(root+_name_file);
    bool existing_disk=info.exists();

    QSqlQuery file(db);
    file.prepare("SELECT Hash, LastMod, Count(*) FROM Files WHERE File='"+_name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения хеша локального файла ") << _name_file;
        log.Write(QString(QString("tDatabaseOp \t GetLocalHash \t ++ ОШИБКА ++ получения хеша локального файла ")+_name_file.toUtf8()));}

    file.next();
    bool existing_base=file.value(2).toInt()>0;

    if(existing_base)
    {
        //есть в базе
        //берем дату-время модификации и хэш из базы
        base_date_time=file.value(1).toDateTime();
        hash=file.value(0).toString();

        if(existing_disk)
        {
            //есть и на диске и в базе
            //проверяем времена
            QDateTime dt=info.lastModified();
            disk_date_time=RoundDateTime(dt);

            if(base_date_time!=disk_date_time)
            {
                //времена не совпадают
                //вычисляем хеш с диска

                tCalcHash ch;
                hash=ch.GetFileHash(root+_name_file);

                //Нужно обновить в локальной таблице файлов у всех моделей которые используют этот файл время модификации и хэш
                //а также и суммарный хеш модели
                UpdateLocalHash(_name_file, disk_date_time, hash);
            }
        }
    }
    else
    {
        //нет в базе
        //проверяем есть ли на диске
        if(existing_disk)
        {
            //нет в базе но есть на диске
            tCalcHash ch;
            hash=ch.GetFileHash(root+_name_file);
        }
    }
    _sending = existing_base && existing_disk;

    _receiving = existing_base && base_date_time==disk_date_time || !existing_disk;


    return hash;
}
//----------------------------------------------------------
void tDatabaseOp::UpdateLocalHash(const QString &_name_file, const QDateTime _disk_date_time, const QString &_hash)
{
    //обновить хеш и время модификации у всех файлов с этим именем

    QSqlQuery upd_hash(db);
    upd_hash.prepare("UPDATE Files SET LastMod='"+_disk_date_time.toString(Qt::ISODate)+"', Hash='"+_hash+"' WHERE File='"+_name_file+"'");
    if(!upd_hash.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления локального хеша ") << _name_file;
        log.Write(QString(QString("tDatabaseOp \t UpdateLocalHash \t ++ ОШИБКА ++ обновления локального хеша ")+_name_file.toUtf8()));}

    //найти все модели которые используют этот файл
    QSqlQuery model(db);
    model.prepare("SELECT Model FROM Files WHERE File='"+_name_file+"'");
    if(!model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения модели для обновления локального хеша ") << _name_file;
        log.Write(QString(QString("tDatabaseOp \t UpdateLocalHash \t ++ ОШИБКА ++ получения модели для обновления локального хеша ")+_name_file.toUtf8()));}

    while(model.next())
    {
        qlonglong num_model=model.value(0).toLongLong();
        QSqlQuery files_model(db);
        files_model.prepare("SELECT hash FROM Files WHERE Model="+QString::number(num_model));
        if(!files_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения файлов для обновления суммарного локального хеша модели ") << num_model;
            log.Write(QString(QString("tDatabaseOp \t UpdateLocalHash \t ++ ОШИБКА ++ получения файлов для обновления суммарного локального хеша модели ")+QString::number(num_model)));}

        tCalcHash ch;
        while(files_model.next())
        {
            QString file_hash=files_model.value(0).toString();
            ch.AddToHash(file_hash.toAscii());
        }
        QString summ_hash=ch.ResultHash();

        //обновить суммарный хэш в записи модели
        QSqlQuery update_model(db);
        update_model.prepare("UPDATE StructModels SET Hash='"+summ_hash+"' FROM  WHERE Num="+QString::number(num_model));
        if(!update_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления суммарного локального хеша модели ") << num_model;
            log.Write(QString(QString("tDatabaseOp \t UpdateLocalHash \t ++ ОШИБКА ++ обновления суммарного локального хеша модели ")+QString::number(num_model)));}

    }
}

//----------------------------------------------------------
QString tDatabaseOp::GetServerHash(const QString& name_file)
{
    QSqlQuery file(db);
    file.prepare("SELECT Hash, Count(*) FROM ServerFiles WHERE File='"+name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения хеша серверного файла ") << name_file;
        log.Write(QString(QString("tDatabaseOp \t GetServerHash \t ++ ОШИБКА ++ получения хеша серверного файла ")+name_file.toUtf8()));}

    file.next();
    //    int N=file.value(1).toInt();
    return file.value(0).toString();
}
//----------------------------------------------------------
void tDatabaseOp::DeleteingFile(const QString &_file_name)
{
    QString relat=_file_name.right(_file_name.length()-root.length());

    QSqlQuery del_file(db);
    del_file.prepare("DELETE FROM Files WHERE File='"+relat+"'");
    if(!del_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления файла ") << relat;
        log.Write(QString(QString("tDatabaseOp \t DeleteingFile \t ++ ОШИБКА ++ удаления файла ")+relat.toUtf8()));}

}
//----------------------------------------------------------
QDateTime tDatabaseOp::GetLastMod(const QString& _file_name)
{
    QSqlQuery sel_last_dt(db);
    sel_last_dt.prepare("SELECT Count(*), LastMod FROM Files WHERE File='"+_file_name+"'");
    if(!sel_last_dt.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки даты-времени модификации файла ") << _file_name;
        log.Write(QString(QString("tDatabaseOp \t GetLastMod \t ++ ОШИБКА ++ выборки даты-времени модификации файла ")+_file_name.toUtf8()));}

    sel_last_dt.next();
    QDateTime dt=sel_last_dt.value(1).toDateTime();
    return dt;
}
//----------------------------------------------------------
void tDatabaseOp::ClearlModConv()
{
    //Сбрасываем поля Send и Delete_local перед началом работы конвейера моделей
    QSqlQuery clear_local_mod(db);
    clear_local_mod.prepare("Update Files Set Send=0, Delete_local=0");
    if(!clear_local_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ сброса Send и Delete_local перед началом работы конвейера моделей ");
        log.Write(QString("tDatabaseOp \t ClearlModConv \t ++ ОШИБКА ++ сброса Send и Delete_local перед началом работы конвейера моделей "));}

    QSqlQuery clear_server_mod(db);
    clear_server_mod.prepare("Update ServerFiles Set Receive=0, Delete_server=0");
    if(!clear_server_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ сброса Receive и Delete_server перед началом работы конвейера моделей ");
        log.Write(QString("tDatabaseOp \t ClearlModConv \t ++ ОШИБКА ++ сброса Receive и Delete_server перед началом работы конвейера моделей "));}

}
//----------------------------------------------------------
void tDatabaseOp::PrepareSendFile(const QString &_file_name)
{
    //мы не смогли бы выбрать в интерфейсе этот файл если бы его небыло в базе
    //поэтому проверять его наличие в базе не буду

    //нужно отметить в поле Send=true
    QSqlQuery upd_send(db);
    upd_send.prepare("UPDATE Files SET Send=1 WHERE File='"+_file_name+"'");
    if(!upd_send.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отметки Send файла ") << _file_name;
        log.Write(QString(QString("tDatabaseOp \t PrepareSendFile \t ++ ОШИБКА ++ отметки Send файла ")+_file_name.toUtf8()));}
}
//----------------------------------------------------------

void tDatabaseOp::PrepareDeletingServer(const QString& _file_name)
{
    //нужно отметить в поле Delete_server=true
    QSqlQuery upd_del_serv(db);
    upd_del_serv.prepare("UPDATE ServerFiles SET Delete_server=1 WHERE File='"+_file_name+"'");
    if(!upd_del_serv.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отметки Delete_server файла ") << _file_name;
        log.Write(QString(QString("tDatabaseOp \t PrepareDeletingServer \t ++ ОШИБКА ++ отметки Delete_server файла ")+_file_name.toUtf8()));}
}
//----------------------------------------------------------
void tDatabaseOp::PrepareReceiveFile(const QString& _file_name)
{
    //нужно отметить Receive=true
    QSqlQuery upd_receive(db);
    upd_receive.prepare("UPDATE ServerFiles SET Receive=1 WHERE File='"+_file_name+"'");
    if(!upd_receive.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отметки Receive файла ") << _file_name;
        log.Write(QString(QString("tDatabaseOp \t PrepareReceiveFile \t ++ ОШИБКА ++ отметки Receive файла ")+_file_name.toUtf8()));}
}
//----------------------------------------------------------
void tDatabaseOp::PrepareDeletingLocal(const QString& _file_name)
{
    //нужно отметить в поле Delete_local=true
    QSqlQuery upd_del_loc(db);
    upd_del_loc.prepare("UPDATE Files SET Delete_local=1 WHERE File='"+_file_name+"'");
    if(!upd_del_loc.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ отметки Delete_local файла ") << _file_name;
        log.Write(QString(QString("tDatabaseOp \t PrepareDeletingLocal \t ++ ОШИБКА ++ отметки Delete_local файла ")+_file_name.toUtf8()));}
}
//----------------------------------------------------------
bool tDatabaseOp::GetNextSendDelModel(QString& _name_model)
{
    QSqlQuery select_send_models(db);
    select_send_models.prepare("SELECT StructModels.Struct, StructModels.Num FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num GROUP BY StructModels.Struct, Files.Send HAVING Files.Send=1");
    if(!select_send_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на отправку моделей ");
        log.Write(QString("tDatabaseOp \t GetNextSendDelModel \t ++ ОШИБКА ++ выборки отмеченных на отправку моделей "));}

    if(select_send_models.next())
    {
        _name_model=select_send_models.value(0).toString();
        return true;
    }
    else
    {
        QSqlQuery select_del_models(db);
        select_del_models.prepare("SELECT ServerStructModels.Struct, ServerStructModels.Num FROM ServerStructModels INNER JOIN ServerFiles ON ServerFiles.Model=ServerStructModels.Num GROUP BY ServerStructModels.Struct, ServerFiles.Delete_server HAVING ServerFiles.Delete_server=1");
        if(!select_del_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на удаление серверных моделей ");
            log.Write(QString("tDatabaseOp \t GetNextSendDelModel \t ++ ОШИБКА ++ выборки отмеченных на удаление серверных моделей "));}

        if(select_del_models.next())
        {
            _name_model=select_del_models.value(0).toString();
            return true;
        }
        else
        {
            return false;
        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::GetSendModelFiles(const QString& _name_model, QStringList& _list_files)
{
    QSqlQuery select_send_files(db);
    select_send_files.prepare("SELECT Files.File FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num WHERE StructModels.Struct='"+_name_model+"' AND Send=1");
    if(!select_send_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на отправку файлов модели ") << _name_model;
        log.Write(QString(QString("tDatabaseOp \t GetSendModelFiles \t ++ ОШИБКА ++ выборки отмеченных на отправку файлов модели ")+_name_model.toUtf8()));}

    while(select_send_files.next())
    {
        QString file_name=select_send_files.value(0).toString();
        _list_files.push_back(file_name);

        //Снять отметку с файлов обработанной модели
        QSqlQuery update_sending_files(db);
        update_sending_files.prepare("UPDATE Files SET Send=0  WHERE File='"+file_name+"'");
        if(!update_sending_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ снятия отметки Send с обработанной модели ") << _name_model;
            log.Write(QString(QString("tDatabaseOp \t GetSendModelFiles \t ++ ОШИБКА ++ снятия отметки Send с обработанной модели ")+_name_model.toUtf8()));}

    }
}
//----------------------------------------------------------
void tDatabaseOp::GetDeleteServerModelFiles(const QString& _name_model, QStringList& _list_files)
{
    QSqlQuery select_server_del_files(db);
    select_server_del_files.prepare("SELECT ServerFiles.File FROM ServerStructModels INNER JOIN ServerFiles ON ServerFiles.Model=ServerStructModels.Num WHERE ServerStructModels.Struct='"+_name_model+"' AND Delete_server=1");
    if(!select_server_del_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на удаление с сервера файлов модели ") << _name_model;
        log.Write(QString(QString("tDatabaseOp \t GetDeleteServerModelFiles \t ++ ОШИБКА ++ выборки отмеченных на удаление с сервера файлов модели ")+_name_model.toUtf8()));}

    while(select_server_del_files.next())
    {
        QString file_name=select_server_del_files.value(0).toString();
        _list_files.push_back(file_name);

        //Снять отметку с файлов обработанной модели
        QSqlQuery update_delete_server_files(db);
        update_delete_server_files.prepare("UPDATE ServerFiles SET Delete_server=0  WHERE File='"+file_name+"'");
        if(!update_delete_server_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ снятия отметки Delete_server с обработанной модели ") << _name_model;
            log.Write(QString(QString("tDatabaseOp \t GetDeleteServerModelFiles \t ++ ОШИБКА ++ снятия отметки Delete_server с обработанной модели ")+_name_model.toUtf8()));}

    }
}
//----------------------------------------------------------
void tDatabaseOp::PrepareUpdateLastSynch(bool _send)
{
    //НАЧАЛО ОБНОВЛЕНИЯ ТАБЛИЦ LAST
    l="tDatabaseOp \tPrepareUpdateLastSynch\tНАЧАЛО ПОДГОТОВКИ ОБНОВЛЕНИЯ LAST ";
    log.Write(l);

    QSqlQuery reset_last(db);
    reset_last.prepare("UPDATE LastStructModels SET Modified=0");
    if(!reset_last.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ сброса отметок Found с Last моделей ");
        log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ сброса отметок Found с Last моделей ")));}

    if(_send)
    {
        log.Write(QString("tDatabaseOp \tUpdateLastSynch\tОбновление Last таблиц по серверным таблицам  "));
        QSqlQuery reset_server_table(db);
        reset_server_table.prepare("UPDATE ServerStructModels SET Modified=0");
        if(!reset_server_table.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ сброса отметок Found с серверных моделей ");
            log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ сброса отметок Found с серверных моделей ")));}
    }
    else
    {
        log.Write(QString("tDatabaseOp \tUpdateLastSynch\tОбновление Last таблиц по локальным таблицам  "));
        QSqlQuery reset_local_table(db);
        reset_local_table.prepare("UPDATE StructModels SET Modified=0");
        if(!reset_local_table.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ сброса отметок Found с локальных моделей ");
            log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ сброса отметок Found с локальных моделей ")));}
    }
}
//----------------------------------------------------------
void tDatabaseOp::UpdateLastSynchMark(const QString& _file_name, bool _server)
{

    //ПРОДОЛЖЕНИЕ ОБНОВЛЕНИЯ ТАБЛИЦ ДФЫЕ

    //Вначале сбросить отметки Found у всех моделей той группы таблиц что нужно, серверной или локальной.
    //сбросить отметки Found у всех Last моделей
    //Выбрать и установить отметки у тех моделей что содержат указаный файл
    //если такой модели вообще не найдено, то отметить соответствующую модель из таблицы Last для последующего удаления
    //повторить для всех переданых, принятых и удаленных файлов
    //пройти по списку отмеченых локальных или серверных моделей и отметить аналогичные модели в таблице Last в поле Found

    //удалить модели из таблицы Last отмеченные в поле Found записи

    //Вновь пройти по всем отмеченым сервреным или локальным моделям и перенести их данные в таблицу Last, а потом и список файлов модели


    if(_server)
    {
        log.Write(QString("tDatabaseOp \tUpdateLastSynch\tОбновление Last таблиц по серверным таблицам  "));

        //получим имя файла моделей, содержащих указанный файл
        QSqlQuery sel_models_file(db);
        sel_models_file.prepare("SELECT DISTINCT Model FROM ServerFiles  WHERE File='"+_file_name+"'");
        if(!sel_models_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения Model серверных моделей содержащих файл ") << _file_name;
            log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ получения Model серверных моделей содержащих файл ")+_file_name.toUtf8()));}

        //установим отметки для последующего копирования
        while(sel_models_file.next())
        {
            qlonglong model=sel_models_file.value(0).toLongLong();
            QSqlQuery update_model_file(db);
            update_model_file.prepare("UPDATE ServerStructModels SET Modified=1 WHERE Num="+QString::number(model));
            if(!update_model_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ установки отметок Found=1 на серверные модели содержащих файл ") << _file_name;
                log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ установки отметок Found=1 на серверные модели содержащих файл ")+_file_name.toUtf8()));}

            QSqlQuery sel_models_file1(db);
            sel_models_file1.prepare("SELECT DISTINCT DiskFile FROM ServerStructModels  WHERE Num="+QString::number(model));
            if(!sel_models_file1.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения DiskFile серверных моделей содержащих файл ") << _file_name;
                log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ получения DiskFile серверных моделей содержащих файл ")+_file_name.toUtf8()));}
            sel_models_file1.next();

            QString disk_file=sel_models_file1.value(0).toString();

            QSqlQuery update_model_file_last(db);
            update_model_file_last.prepare("UPDATE LastStructModels SET Modified=1 WHERE DiskFile='"+disk_file+"'");
            if(!update_model_file_last.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ установки отметок Found=1 на Last модели содержащих файл ") << _file_name;
                log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ установки отметок Found=1 на Last модели содержащих файл ")+_file_name.toUtf8()));}

        }


    }
    else
    {
        log.Write(QString("tDatabaseOp \tUpdateLastSynch\tОбновление Last таблиц по локальным таблицам  "));

        //получим имя файла моделей, содержащих указанный файл
        QSqlQuery sel_models_file(db);
        sel_models_file.prepare("SELECT DISTINCT Model FROM Files  WHERE File='"+_file_name+"'");
        if(!sel_models_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения Model локальных моделей содержащих файл ") << _file_name;
            log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ получения Model локальных моделей содержащих файл ")+_file_name.toUtf8()));}



        //установим отметки для последующего копирования
        while(sel_models_file.next())
        {
            qlonglong model=sel_models_file.value(0).toLongLong();

            QSqlQuery sel_models_file1(db);
            sel_models_file1.prepare("SELECT DiskFile FROM StructModels WHERE Num="+QString::number(model));
            if(!sel_models_file1.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения DiskFile локальных моделей содержащих файл ") << _file_name;
                log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ получения DiskFile локальных моделей содержащих файл ")+_file_name.toUtf8()));}
            sel_models_file1.next();
            QString disk_file=sel_models_file1.value(0).toString();


            QSqlQuery update_model_file(db);
            update_model_file.prepare("UPDATE StructModels SET Modified=1 WHERE Num="+QString::number(model));
            if(!update_model_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ установки отметок Found=1 на локальные модели содержащих файл ") << _file_name;
                log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ установки отметок Found=1 на локальные модели содержащих файл ")+_file_name.toUtf8()));}


            QSqlQuery update_model_file_last(db);
            update_model_file_last.prepare("UPDATE LastStructModels SET Modified=1 WHERE DiskFile='"+disk_file+"'");
            if(!update_model_file_last.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ установки отметок Found=1 на Last модели содержащих файл ") << _file_name;
                log.Write(QString(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ установки отметок Found=1 на Last модели содержащих файл ")+_file_name.toUtf8()));}

        }

    }

}
//----------------------------------------------------------
void tDatabaseOp::ExecUpdateLastSynch(bool _server)
{
    //окончание механизма обновления таблиц LAST

    l="tDatabaseOp \tPrepareUpdateLastSynch\tОКОНЧАНИЕ ОБНОВЛЕНИЯ LAST ";
    log.Write(l);

    QSqlQuery del_file(db);
    del_file.prepare("DELETE FROM LastStructModels WHERE Modified=1");
    if(!del_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления отмеченых моделей из Last ");
        log.Write(QString("tDatabaseOp \t UpdateLastSynch \t ++ ОШИБКА ++ установки отметок Found=1 на Last модели по серверной таблице "));}

    if(_server)
    {
        QSqlQuery is_not_file(db);
        is_not_file.prepare("SELECT Count(*) FROM ServerStructModels WHERE Modified=1");
        if(!is_not_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки наличия файлов для передачи");}
        is_not_file.next();
        int c=is_not_file.value(0).toInt();

        if(c!=0)
        {
            QSqlQuery ls_model(db);
            ls_model.prepare("SELECT Num, DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash FROM ServerStructModels WHERE Modified=1");
            if(!ls_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки модели для передачи");}

            while(ls_model.next())
            {

                qlonglong num_mod=ls_model.value(0).toLongLong();
                QString disk_file=ls_model.value(1).toString();

                QString Title=ls_model.value(2).toString();
                QString Description=ls_model.value(3).toString();
                QString Struct=ls_model.value(4).toString();
                QDateTime LastMod=ls_model.value(5).toDateTime();
                QString Hash=ls_model.value(6).toString();
                QDateTime ListFilesLastMod=ls_model.value(7).toDateTime();
                QString ListFilesHash=ls_model.value(8).toString();
                QString SummListHash=ls_model.value(9).toString();

                QSqlQuery insert_synch_model(db);
                insert_synch_model.prepare("INSERT INTO LastStructModels (DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash, Found) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                insert_synch_model.bindValue(0, disk_file);
                insert_synch_model.bindValue(1, Title);
                insert_synch_model.bindValue(2, Description);
                insert_synch_model.bindValue(3, Struct);
                insert_synch_model.bindValue(4, LastMod);
                insert_synch_model.bindValue(5, Hash);
                insert_synch_model.bindValue(6, ListFilesLastMod);
                insert_synch_model.bindValue(7, ListFilesHash);
                insert_synch_model.bindValue(8, SummListHash);
                insert_synch_model.bindValue(9, 1);
                if(!insert_synch_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления в таблицу последнего состояния моделей ") << disk_file;}
                qlonglong num_last_mod=insert_synch_model.lastInsertId().toLongLong();

                //теперь обойдем все файлы модели локальной таблицы и перенесем (обновим) данные таблицы файлов последнего состояния
                QSqlQuery sel_loc_file_mod(db);
                sel_loc_file_mod.prepare("SELECT File, Size, LastMod, Hash FROM ServerFiles WHERE Model="+QString::number(num_mod));
                if(!sel_loc_file_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки файлов модели ") << num_mod;}
                while(sel_loc_file_mod.next())
                {
                    QString File=sel_loc_file_mod.value(0).toString();
                    qlonglong Size=sel_loc_file_mod.value(1).toLongLong();
                    QDateTime LastMod=sel_loc_file_mod.value(2).toDateTime();
                    QString Hash=sel_loc_file_mod.value(3).toString();


                    QSqlQuery ins_last_files(db);
                    ins_last_files.prepare("INSERT INTO LastFiles (Model, File, Size, LastMod, Hash, Found) VALUES (?, ?, ?, ?, ?, ?)");

                    ins_last_files.bindValue(0, num_last_mod);
                    ins_last_files.bindValue(1, File);
                    ins_last_files.bindValue(2, Size);
                    ins_last_files.bindValue(3, LastMod);
                    ins_last_files.bindValue(4, Hash);
                    ins_last_files.bindValue(5, 1);

                    if(!ins_last_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления последнего состояния файлов модели ") << num_last_mod;}
                }

            }
        }
    }
    else
    {
        QSqlQuery is_not_file(db);
        is_not_file.prepare("SELECT Count(*) FROM StructModels WHERE Modified=1");
        if(!is_not_file.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки наличия файлов для передачи");}
        is_not_file.next();
        int c=is_not_file.value(0).toInt();

        if(c!=0)
        {
            QSqlQuery ls_model(db);
            ls_model.prepare("SELECT Num, DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash FROM StructModels WHERE Modified=1");
            if(!ls_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки модели для передачи");}

            while(ls_model.next())
            {

                qlonglong num_mod=ls_model.value(0).toLongLong();
                QString disk_file=ls_model.value(1).toString();

                QString Title=ls_model.value(2).toString();
                QString Description=ls_model.value(3).toString();
                QString Struct=ls_model.value(4).toString();
                QDateTime LastMod=ls_model.value(5).toDateTime();
                QString Hash=ls_model.value(6).toString();
                QDateTime ListFilesLastMod=ls_model.value(7).toDateTime();
                QString ListFilesHash=ls_model.value(8).toString();
                QString SummListHash=ls_model.value(9).toString();

                QSqlQuery insert_synch_model(db);
                insert_synch_model.prepare("INSERT INTO LastStructModels (DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash, Found) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
                insert_synch_model.bindValue(0, disk_file);
                insert_synch_model.bindValue(1, Title);
                insert_synch_model.bindValue(2, Description);
                insert_synch_model.bindValue(3, Struct);
                insert_synch_model.bindValue(4, LastMod);
                insert_synch_model.bindValue(5, Hash);
                insert_synch_model.bindValue(6, ListFilesLastMod);
                insert_synch_model.bindValue(7, ListFilesHash);
                insert_synch_model.bindValue(8, SummListHash);
                insert_synch_model.bindValue(9, 1);
                if(!insert_synch_model.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления в таблицу последнего состояния моделей ") << disk_file;}
                qlonglong num_last_mod=insert_synch_model.lastInsertId().toLongLong();

                //теперь обойдем все файлы модели локальной таблицы и перенесем (обновим) данные таблицы файлов последнего состояния
                QSqlQuery sel_loc_file_mod(db);
                sel_loc_file_mod.prepare("SELECT File, Size, LastMod, Hash FROM Files WHERE Model="+QString::number(num_mod));
                if(!sel_loc_file_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки файлов модели ") << num_mod;}
                while(sel_loc_file_mod.next())
                {
                    QString File=sel_loc_file_mod.value(0).toString();
                    qlonglong Size=sel_loc_file_mod.value(1).toLongLong();
                    QDateTime LastMod=sel_loc_file_mod.value(2).toDateTime();
                    QString Hash=sel_loc_file_mod.value(3).toString();


                    QSqlQuery ins_last_files(db);
                    ins_last_files.prepare("INSERT INTO LastFiles (Model, File, Size, LastMod, Hash, Found) VALUES (?, ?, ?, ?, ?, ?)");

                    ins_last_files.bindValue(0, num_last_mod);
                    ins_last_files.bindValue(1, File);
                    ins_last_files.bindValue(2, Size);
                    ins_last_files.bindValue(3, LastMod);
                    ins_last_files.bindValue(4, Hash);
                    ins_last_files.bindValue(5, 1);

                    if(!ins_last_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления последнего состояния файлов модели ") << num_last_mod;}
                }

            }
        }
    }
}
//----------------------------------------------------------
bool tDatabaseOp::GetNextReceiveDelModel(QString& _name_model)
{
    QSqlQuery select_rec_models(db);
    select_rec_models.prepare("SELECT ServerStructModels.Struct, ServerStructModels.Num FROM ServerStructModels INNER JOIN ServerFiles ON ServerFiles.Model=ServerStructModels.Num GROUP BY ServerStructModels.Struct, ServerFiles.Receive HAVING ServerFiles.Receive=1");
    if(!select_rec_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на прием моделей ");
        log.Write(QString("tDatabaseOp \t GetNextReceiveDelModel \t ++ ОШИБКА ++ выборки отмеченных на прием моделей "));}

    if(select_rec_models.next())
    {
        _name_model=select_rec_models.value(0).toString();
        return true;
    }
    else
    {
        QSqlQuery select_del_models(db);
        select_del_models.prepare("SELECT StructModels.Struct, StructModels.Num FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num GROUP BY StructModels.Struct, Files.Delete_local HAVING Files.Delete_local=1");
        if(!select_del_models.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на удаление локальных моделей ");
            log.Write(QString("tDatabaseOp \t GetNextReceiveDelModel \t ++ ОШИБКА ++ выборки отмеченных на удаление локальных моделей "));}
        if(select_del_models.next())
        {
            _name_model=select_del_models.value(0).toString();
            return true;
        }
        else
        {
            return false;
        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::GetReceiveModelFiles(const QString& _name_model, QStringList& _list_files)
{

    QSqlQuery select_rec_files(db);
    select_rec_files.prepare("SELECT ServerFiles.File FROM ServerStructModels INNER JOIN ServerFiles ON ServerFiles.Model=ServerStructModels.Num WHERE ServerStructModels.Struct='"+_name_model+"' AND Receive=1");
    if(!select_rec_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на получение файлов модели ") << _name_model;
        log.Write(QString(QString("tDatabaseOp \t GetReceiveModelFiles \t ++ ОШИБКА ++ выборки отмеченных на получение файлов модели ")+_name_model.toUtf8()));}

    while(select_rec_files.next())
    {
        QString file_name=select_rec_files.value(0).toString();
        _list_files.push_back(file_name);

        //Снять отметку с файлов обработанной модели
        QSqlQuery update_receive_files(db);
        update_receive_files.prepare("UPDATE ServerFiles SET Receive=0  WHERE File='"+file_name+"'");
        if(!update_receive_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ снятия отметки Receive с обработанной модели ") << _name_model;
            log.Write(QString(QString("tDatabaseOp \t GetReceiveModelFiles \t ++ ОШИБКА ++ снятия отметки Receive с обработанной модели ")+_name_model.toUtf8()));}

    }
}
//----------------------------------------------------------
void tDatabaseOp::UpdateServerTable(const QString &_name)
{
    l="tDatabaseOp \tUpdateServerTable\tОбновление серверных таблиц  ";
    log.Write(l);

    //найти из локальных файлов время модификации и хеш по имени файла
    QSqlQuery select_loc_date_hash(db);
    select_loc_date_hash.prepare("SELECT LastMod, Hash FROM Files WHERE File='"+_name+"'");
    if(!select_loc_date_hash.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ времени и хеша локального файла ") << _name;
        log.Write(QString(QString("tDatabaseOp \t UpdateServerTable \t ++ ОШИБКА ++ времени и хеша локального файла ")+_name.toUtf8()));}

    select_loc_date_hash.next();
    QDateTime last_mod=select_loc_date_hash.value(0).toDateTime();
    QString hash=select_loc_date_hash.value(1).toString();

    //обновить в серверных таблицах время и хеш по имени файла
    QSqlQuery update_serv_date_hash(db);
    update_serv_date_hash.prepare("UPDATE ServerFiles SET LastMod='"+last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"' WHERE File='"+_name+"'");
    if(!update_serv_date_hash.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления времени и хеша серверного файла ") << _name;
        log.Write(QString(QString("tDatabaseOp \t UpdateServerTable \t ++ ОШИБКА ++ обновления времени и хеша серверного файла ")+_name.toUtf8()));}

    //найти список серверных моделей использующих этот файл
    QSqlQuery sel_serv_mod(db);
    sel_serv_mod.prepare("SELECT Model FROM ServerFiles WHERE File='"+_name+"'");
    if(!sel_serv_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения списка серверных моделей, использующих файл ") << _name;
        log.Write(QString(QString("tDatabaseOp \t UpdateServerTable \t ++ ОШИБКА ++ получения списка серверных моделей, использующих файл ")+_name.toUtf8()));}

    while(sel_serv_mod.next())
    {
        qlonglong num_mod=update_serv_date_hash.value(0).toLongLong();
        tCalcHash ch;
        //пересчитать суммарный хеш модели
        QSqlQuery serv_files_mod(db);
        serv_files_mod.prepare("SELECT Hash FROM ServerFiles WHERE Model="+QString::number(num_mod));
        if(!serv_files_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения списка хешей файлов модели ") << num_mod;
            log.Write(QString(QString("tDatabaseOp \t UpdateServerTable \t ++ ОШИБКА ++ получения списка хешей файлов модели ")+QString::number(num_mod)));}

        while(serv_files_mod.next())
        {

            QString file_hash=serv_files_mod.value(0).toString();
            ch.AddToHash(file_hash.toAscii());
        }
        QString summ_hash=ch.ResultHash();

        //обновить в записи модели суммарный хэш
        QSqlQuery update_summ_hash_mod(db);
        update_summ_hash_mod.prepare("UPDATE ServerStructModels SET SummListHash='"+summ_hash+"' WHERE Num="+QString::number(num_mod));
        if(!update_summ_hash_mod.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления суммарного хеша серверной модели ") << num_mod;
            log.Write(QString(QString("tDatabaseOp \t UpdateServerTable \t ++ ОШИБКА ++ получения списка хешей файлов модели ")+QString::number(num_mod)));}

    }
}
//----------------------------------------------------------
void tDatabaseOp::GetDeleteLocalModelFiles(const QString& _name_model, QStringList& _list_files)
{
    QSqlQuery select_local_del_files(db);
    select_local_del_files.prepare("SELECT Files.File FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num WHERE StructModels.Struct='"+_name_model+"' AND Delete_local=1");
    if(!select_local_del_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ выборки отмеченных на удаление с локали файлов модели ") << _name_model;
        log.Write(QString(QString("tDatabaseOp \t GetDeleteLocalModelFiles \t ++ ОШИБКА ++ выборки отмеченных на удаление с локали файлов модели ")+_name_model.toUtf8()));}

    while(select_local_del_files.next())
    {
        QString file_name=select_local_del_files.value(0).toString();
        _list_files.push_back(file_name);

        //Снять отметку с файлов обработанной модели
        QSqlQuery update_delete_local_files(db);
        update_delete_local_files.prepare("UPDATE Files SET Delete_local=0  WHERE File='"+file_name+"'");
        if(!update_delete_local_files.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ снятия отметки Delete_local с обработанной модели ") << _name_model;
            log.Write(QString(QString("tDatabaseOp \t GetDeleteLocalModelFiles \t ++ ОШИБКА ++ снятия отметки Delete_local с обработанной модели ")+_name_model.toUtf8()));}

    }
}
//----------------------------------------------------------
void tDatabaseOp::SaveLoginPassword(QString &_login, QString &_password, bool _new_user, qlonglong _s_num)
{
    l="tDatabaseOp \tSaveLoginPassword\t Обработка логина "+_login.toUtf8();
    log.Write(l);

    if(_new_user)
    {
        l="tDatabaseOp \tSaveLoginPassword\t Новый логин ";
        log.Write(l);

        //новый пользователь
        QSqlQuery insert_new_user(db);
        insert_new_user.prepare("INSERT INTO Logins (Num, Login, PassHash, NoDelete) VALUES (?, ?, ?, ?)");

        insert_new_user.bindValue(0, _s_num);
        insert_new_user.bindValue(1, _login);

        tCalcHash ch;
        ch.AddToHash(_password.toAscii());
        insert_new_user.bindValue(2, ch.ResultHash());
        insert_new_user.bindValue(3, 0);

        if(!insert_new_user.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления пользователя ") << _login;
        log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ добавления пользователя ")+_login.toUtf8()));}


    }
    else
    {
        l="tDatabaseOp \tSaveLoginPassword\t Имеющийся логин ";
        log.Write(l);

        //пользователь уже есть
        QSqlQuery is_log_present(db);
        is_log_present.prepare("SELECT Count(*) FROM Logins WHERE Num="+QString::number(_s_num));
        if(!is_log_present.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ проверки новизны нового логина ") << _login;
        log.Write(QString(QString("tDatabaseOp \t GetDeleteLocalModelFiles \t ++ ОШИБКА ++ проверки новизны нового логина ")+_login.toUtf8()));}
        is_log_present.next();

        int c=is_log_present.value(0).toInt();
        if(c==0)
        {
            //такого пользователя нет!
//            ret="Login "+_login+"(номер "+QString::number(s_num)+") не отредактирован.\nТакого пользователя нет!\nРедактирование невозможно!";
            l="tDatabaseOp \tSaveLoginPassword\t Такого пользователя нет! ";
            log.Write(l);
        }
        else
        {
            //пользователь есть
            tCalcHash ch;
            ch.AddToHash(_password.toAscii());
            QSqlQuery update_user(db);
            update_user.prepare("UPDATE Logins SET Login='"+_login+"', PassHash='"+ch.ResultHash()+"' WHERE Num="+QString::number(_s_num));
            if(!update_user.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ обновления логина ") << _login;
            log.Write(QString(QString("tDatabaseOp \t GetDeleteLocalModelFiles \t ++ ОШИБКА ++ обновления логина ")+_login.toUtf8()));}
        }
    }
}
//----------------------------------------------------------
QStringList tDatabaseOp::GetLoginsList()
{
    QStringList ret;

    QSqlQuery get_logins(db);
    get_logins.prepare("SELECT Login FROM Logins ORDER BY Num, Login");
    if(!get_logins.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения списка логинов ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ получения списка логинов ")));}

    while(get_logins.next())
    {
        QString login=get_logins.value(0).toString();
        ret.push_back(login);
    }


    return ret;
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
qlonglong tDatabaseOp::GetNumLogin(int _row)
{
    qlonglong num_log;

    QSqlQuery search_login_num(db);
    search_login_num.prepare("SELECT Num FROM Logins ORDER BY Num, Login");
    if(!search_login_num.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ получения номера логина ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ получения номера логина ")));}

    for(int i=0; i<_row+1;i++)
    {
        search_login_num.next();
    }

    num_log=search_login_num.value(0).toLongLong();


    return num_log;
}
//----------------------------------------------------------
void tDatabaseOp::DeleteLogin(qlonglong _num_log)
{
    QSqlQuery del_login(db);
    del_login.prepare("DELETE FROM Logins WHERE Num="+QString::number(_num_log));
    if(!del_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления пользователя ")+QString::number(_num_log);
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ удаления пользователя ")+QString::number(_num_log)));}
}
//----------------------------------------------------------
void tDatabaseOp::UpdateLogins(QByteArray &_block)
{
    l="tDatabaseOp \tUpdateLogins\tОбновление таблицы логинов  ";
    log.Write(l);

    QDataStream out(&_block, QIODevice::ReadOnly);

//    out.device()->seek(4);
    int num_logins=-1;
    out >> num_logins;

    db.transaction();

    QSqlQuery set_found(db);
    set_found.prepare("UPDATE Logins SET Found=1");
    if(!set_found.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ установки Found=1 ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ установки Found=1 ")));}


    for(int i=0; i<num_logins; i++)
    {
        qlonglong Num=0;
        QString Login="";
        QString PassHash="";
        int NoDelete=0;
        int Writable=0;

        out >> Num;
        out >> Login;
        out >> PassHash;
        out >> NoDelete;
        out >> Writable;

        QSqlQuery select_num_login(db);
        select_num_login.prepare("SELECT Count(*) FROM Logins WHERE Num="+QString::number(Num));
        if(!select_num_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ поиска логина по номеру ")+QString::number(Num);
        log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ поиска логина по номеру ")+QString::number(Num)));}

        select_num_login.next();
        int count=select_num_login.value(0).toInt();
        if(count==0)
        {
            //новый пользователь
            QSqlQuery insert_login(db);
            insert_login.prepare("INSERT INTO Logins (Num, Login, PassHash, NoDelete, Writable, Found) VALUES (?, ?, ?, ?, ?, ?)");
            insert_login.bindValue(0, Num);
            insert_login.bindValue(1, Login);
            insert_login.bindValue(2, PassHash);
            insert_login.bindValue(3, NoDelete);
            insert_login.bindValue(4, Writable);
            insert_login.bindValue(5, 0);

            if(!insert_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ добавления нового логина ")+QString::number(Num);
            log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ поиска логина по номеру ")+QString::number(Num)));}
        }
        else
        {
            //имеющийся пользователь
            QSqlQuery update_login(db);
            update_login.prepare("UPDATE Logins SET Login='"+Login+"', PassHash='"+PassHash+"', NoDelete="+QString::number(NoDelete)+", Writable="+QString::number(Writable)+", Found=0 WHERE NUm="+QString::number(Num));
            if(!update_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ редактирования логина ")+QString::number(Num);
            log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ редактирования логина ")+QString::number(Num)));}

        }
    }

    QSqlQuery delete_login(db);
    delete_login.prepare("DELETE FROM Logins WHERE Found=1");
    if(!delete_login.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ удаления лишних логинов ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ удаления лишних логинов ")));}

    QSqlQuery unset_found(db);
    unset_found.prepare("UPDATE Logins SET Found=0");
    if(!unset_found.exec()){qDebug() << QString::fromUtf8("++ ОШИБКА ++ установки Found=0 ");
    log.Write(QString(QString("tDatabaseOp \t SaveLoginPassword \t ++ ОШИБКА ++ установки Found=0 ")));}

    db.commit();


}
