#include "tDatabaseOp.h"
#include "tSettings.h"
#include <QMessageBox>
#include "tCalcHash.h"

extern tSettings my_settings;
//----------------------------------------------------------------------
tDatabaseOp::tDatabaseOp(const QSqlDatabase &_db)
{
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
    QString root=my_settings.GetRoot();

    //Начало работы с базой данных
    //Начало транзакции
//    bool b=db.transaction();

    QSqlQuery unset_found_models(db);
    unset_found_models.prepare("UPDATE StructModels SET Found=0");

    if(!unset_found_models.exec()){qDebug() << QString::fromUtf8("Ошибка при сбросе Found моделей ");}

    QSqlQuery unset_found_files(db);
    unset_found_files.prepare("UPDATE Files SET Found=0, Del=0");

    if(!unset_found_files.exec()){qDebug() << QString::fromUtf8("Ошибка при сбросе Found и Del файлов ");}

    SearchInfo(root);

    //После обработки всех файлов и всей базы можно удалить из базы более не нужные модели у которых Found=0 (они не найдены на диске)
    QSqlQuery delete_unfound_models(db);
    delete_unfound_models.prepare("DELETE FROM StructModels WHERE Found=0");
    if(!delete_unfound_models.exec()){qDebug() << QString::fromUtf8("Ошибка удалении не найденых на диске моделей ");}

    QSqlQuery delete_unfound_files(db);
    delete_unfound_files.prepare("DELETE FROM Files WHERE Found=0 OR Del=1");
    if(!delete_unfound_files.exec()){qDebug() << QString::fromUtf8("Ошибка удалении не найденых на диске файлов ");}


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
            //проверяем есть ли такое в базе
            QString relat=_folder.right(_folder.length()-root.length());
            QSqlQuery search_file(db);
            QString S="SELECT count(*) FROM StructModels WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
            search_file.prepare(S);

            if(!search_file.exec()){qDebug() << QString::fromUtf8("Ошибка при поиске ") << NormalizePathFiles(relat+"/"+entry);}
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

                if(!struct_models.exec()){qDebug() << QString::fromUtf8("Ошибка при добавлении данных ") << relat+"/"+entry;}

                //добавление списка файлов модели
                //передаем значение автоинкрементного ключа

                AddModelFiles(struct_models.lastInsertId().toLongLong(), _folder, entry, list_files_date_time, list_files_hash, summ_list_hash);

                //Добавляем дату-время файла модели, хеш сумму самого файла модели и сумму сумм файлов в ней описаных
                QSqlQuery update_struct_models(db);

                QString dt=list_files_date_time.toString(Qt::ISODate);

                QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+list_files_hash+"', SummListHash='"+summ_list_hash+"' WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                update_struct_models.prepare(sql);

                if(!update_struct_models.exec()){qDebug() << QString::fromUtf8("Ошибка при обновлении новых данных ") << relat+"/"+entry;}
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

                    if(!search_file.exec()){qDebug() << QString::fromUtf8("Ошибка при поиске ") << NormalizePathFiles(relat+"/"+entry);}
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

                        if(!search_file.exec()){qDebug() << QString::fromUtf8("Ошибка при редактировании ") << NormalizePathFiles(relat+"/"+entry);}
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

                    QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+u_list_file_hash+"', SummListHash='"+summ_list_hash+"', Found=1 WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
                    update_struct_models.prepare(sql);

                    if(!update_struct_models.exec()){qDebug() << QString::fromUtf8("Ошибка при обновлении имеющихся данных ") << relat+"/"+entry;}

                }
                else
                {
                    //модели на диске уже нет
                    //поставить пометку Found=false
                    QSqlQuery update_found_false(db);
                    update_found_false.prepare("UPDATE StructModels SET Found=0 Where DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'");
                    if(!update_found_false.exec()){qDebug() << QString::fromUtf8("Ошибка при пометки на удаление модели ") << NormalizePathFiles(relat+"/"+entry);}

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
        qDebug() << QString::fromUtf8(file_path.toAscii());

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

            if(!files_model.exec()){qDebug() << QString::fromUtf8("Ошибка при записи файла списка ") << root+file_path;}
        }
    }


    list_files.close();
    _summ_list_hash=calc_summ_hash.ResultHash();
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


    //Реализовать добавление новых файлов к существующей модели
    //А также проверку наличия файлов с учетом номера модели
    //Одинаковые Файлы в папке Common должны добавляться для каждой модели отдельно

    tCalcHash ch_model;
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

    if(info_list.exists())
    {
        //список файлов на диске есть
        QFile list_files(model_structs);
        if(!list_files.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << QString::fromUtf8("Не удалось открыть файл списка (обновление) ") << model_structs;
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
            if(!search_file.exec()){qDebug() << QString::fromUtf8("Ошибка отпределения наличия файла для обновления ") << file_path;}
            search_file.next();
            QString hash="";
            int c=search_file.value(0).toInt();
            if(c!=0)
            {
                //файл в базе есть
                //получим дату-время модификации из базы.
                QSqlQuery dt_file(db);
                dt_file.prepare("SELECT LastMod FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                if(!dt_file.exec()){qDebug() << QString::fromUtf8("Ошибка получения даты-времени модификации файла для обновления ") << relat_path;}
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
                        if(!update_file.exec()){qDebug() << QString::fromUtf8("Ошибка обновления даты-времени модификации и хеш-суммы файла ") << relat_path;}

                    }
                    else
                    {
                        //прочитать из базы хеш-сумму
                        QSqlQuery search_hash(db);
                        search_hash.prepare("SELECT Hash FROM Files WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!search_hash.exec()){qDebug() << QString::fromUtf8("Ошибка получения хеш-суммы файла из базы ") << relat_path;}
                        search_hash.next();

                        hash=search_hash.value(0).toString();

                        //отметить файл как существующий
                        QSqlQuery check_found_file(db);
                        check_found_file.prepare("UPDATE Files SET Found=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                        if(!check_found_file.exec()){qDebug() << QString::fromUtf8("Ошибка обновления наличия файла ") << relat_path;}
                    }
                }
                else
                {
                    //файла на диске нет
                    //пометить для удаления эту запись
                    QSqlQuery del_check(db);
                    del_check.prepare("UPDATE Files SET Del=1 WHERE File='"+file_path+"' AND Model="+QString::number(_num));
                    if(!del_check.exec()){qDebug() << QString::fromUtf8("Ошибка пометки на удаление ") << file_path;}
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

                    if(!insert_file.exec()){qDebug() << QString::fromUtf8("Ошибка добавления нового файла ") << file_path;}



                }

            }
            ch_model.AddToHash(hash.toAscii());
        }
        list_files.close();
        _summ_list_hash=ch_model.ResultHash();
    }



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
    if(!mod_select.exec()){qDebug() << QString::fromUtf8("Ошибка поиска файла модели  ") << relat << _num;}
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
            if(!select_date.exec()){qDebug() << QString::fromUtf8("Ошибка поиска даты-времени модификации файла модели  ") << relat << _num;}
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
                if(!mod_check.exec()){qDebug() << QString::fromUtf8("Ошибка пометки Found=1 файла модели  ") << relat << _num;}
            }
            else
            {
                //файл изменялся

                //обновить дату-время, хэш и отметить как существующий
                tCalcHash ch;
                QString hash=ch.GetFileHash(_path_file);

                QSqlQuery update_dt_hash(db);
                update_dt_hash.prepare("UPDATE Files SET LastMod='"+RoundDateTime(disk_last_mod).toString(Qt::ISODate)+"', Hash='"+hash+"', Size="+QString::number(info.size())+", Found=1 WHERE File='"+relat+"' AND Model="+QString::number(_num));
                if(!update_dt_hash.exec()){qDebug() << QString::fromUtf8("Ошибка обновления даты-времени и хеша файла модели  ") << relat << _num;}
            }

        }
        else
        {
            //файла нет на диске
            //Отметить как не существующий
            QSqlQuery set_unfound(db);
            set_unfound.prepare("UPDATE Files SET Found=0 WHERE File='"+relat+"' AND Model="+QString::number(_num));
            if(!set_unfound.exec()){qDebug() << QString::fromUtf8("Ошибка отметки как несуществующего файла модели  ") << relat << _num;}
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

            if(!insert_file.exec()){qDebug() << QString::fromUtf8("Ошибка добавления нового файла модели ") << relat << _num;}
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
        QString suffix=info.suffix();
        if(suffix=="bmp" | suffix=="jpg" | suffix=="jpeg" | suffix=="gif" | suffix=="png")
        {
            //проверяем есть ли такой файл в базе
            QSqlQuery select_info_files(db);
            select_info_files.prepare("SELECT count(*) FROM Files WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
            if(!select_info_files.exec()){qDebug() << QString::fromUtf8("Ошибка поиска файла в папке .info ") << relat+"/"+entry << _num;}
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
                    if(!select_date.exec()){qDebug() << QString::fromUtf8("Ошибка поиска даты-времени модификации файла в папке .info  ") << relat << _num;}
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
                        if(!mod_check.exec()){qDebug() << QString::fromUtf8("Ошибка пометки Found=1 файла в папке .info  ") << relat << _num;}
                    }
                    else
                    {
                        //Файл изменялся
                        //откорректируем дату-время, хэш файла и отметим его как существующий
                        tCalcHash ch;
                        QString hash=ch.GetFileHash(info_folder+"/"+entry);

                        QSqlQuery update_dt_hash(db);

                        update_dt_hash.prepare("UPDATE Files SET LastMod='"+disk_last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, Size="+QString::number(info.size())+" WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                        if(!update_dt_hash.exec()){qDebug() << QString::fromUtf8("Ошибка обновления даты-времени и хеша файла в папке .info  ") << relat << _num;}
                    }
                }
                else
                {
                    //такого файла на диске нет

                    //пометить как несуществующий
                    QSqlQuery set_unfound(db);
                    set_unfound.prepare("UPDATE Files SET Found=0 WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
                    if(!set_unfound.exec()){qDebug() << QString::fromUtf8("Ошибка отметки как несуществующего файла в папке .info  ") << relat << _num;}
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
                insert_info_files.bindValue(1, relat+"/"+entry);
                insert_info_files.bindValue(2, last_mod);
                insert_info_files.bindValue(3, hash);
                insert_info_files.bindValue(4, 1);
                insert_info_files.bindValue(5, info.size());

                if(!insert_info_files.exec()){qDebug() << QString::fromUtf8("Ошибка добавления графических инфо-файлов ") << _folder_mod+"/"+entry;}
            }
        }
    }
}
//----------------------------------------------------------
void tDatabaseOp::ClearModels()
{
    QSqlQuery delete_unfound_models(db);
    delete_unfound_models.prepare("DELETE FROM StructModels WHERE Found=0");
    if(!delete_unfound_models.exec()){qDebug() << QString::fromUtf8("Ошибка удалении не найденых на диске моделей ");}
}
//----------------------------------------------------------
void tDatabaseOp::GetListModels(QDataStream &_out)
{
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
    //}

    //}

    //Определим количество моделей (вообще потом запрос будет сложнее с учетом прав доступа на чтение моделей, пока берем все модели)
    QSqlQuery count_mod(db);
    count_mod.prepare("SELECT Count(*) FROM StructModels");
    if(!count_mod.exec()){qDebug() << QString::fromUtf8("Ошибка определения числа передаваемых моделей ");}
    count_mod.next();
    int cm=count_mod.value(0).toInt();
    _out << cm;

    QSqlQuery models(db);
    models.prepare("SELECT DiskFile, Title, Description, Struct, LastMod, Hash, ListFilesLastMod, ListFilesHash, SummListHash, Num FROM StructModels");
    if(!models.exec()){qDebug() << QString::fromUtf8("Ошибка получения данных передаваемых моделей ");}
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
        if(!count_files.exec()){qDebug() << QString::fromUtf8("Ошибка получения количества файлов модели ") << Num_S;}
        count_files.next();
        int cf=count_files.value(0).toInt();

        _out << cf;

        //Получение списка файлов модели
        QSqlQuery files(db);
        files.prepare("SELECT File, Size, LastMod, Hash, Num FROM Files WHERE Model="+QString::number(Num_S));
        if(!files.exec()){qDebug() << QString::fromUtf8("Ошибка получения списка файлов модели ") << Num_S;}
//        files.next();
        while(files.next())
        {
            QString File=files.value(0).toString();
            qlonglong Size=files.value(1).toLongLong();
            QDateTime LastMod_F=files.value(2).toDateTime();
            QString Hash_F=files.value(3).toString();
            qlonglong Num_F=files.value(4).toLongLong();

            _out << File;
            _out << Size;
            _out << LastMod_F;
            _out << Hash_F;
            _out << Num_F;
        }

    }
}
//----------------------------------------------------------
void tDatabaseOp::GetLocalModelFiles(const QString &_str, QStringList &_list)
{
    _list.clear();
    QSqlQuery list_model_structs(db);
    QString s="SELECT Files.File FROM StructModels INNER JOIN Files ON Files.Model=StructModels.Num WHERE StructModels.Struct='"+_str+"'";
            list_model_structs.prepare(s);
    if(!list_model_structs.exec()){qDebug() << QString::fromUtf8("Ошибка выборки файлов модели ") << _str;}
//    list_model_structs.next();
    while(list_model_structs.next())
    {
        QString File=list_model_structs.value(0).toString();
        _list.push_back(File);
    }
}
//----------------------------------------------------------
QString tDatabaseOp::GetHash(const QString& name_file) const
{
    QSqlQuery file(db);
    file.prepare("SELECT Hash, Count(*) FROM Files WHERE File='"+name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("Ошибка получения хеша файла ") << name_file;}

    file.next();
//    int N=file.value(1).toInt();
    return file.value(0).toString();
}
//----------------------------------------------------------
QDateTime tDatabaseOp::GetLastMod(const QString& name_file) const
{
    QSqlQuery file(db);
    file.prepare("SELECT LastMod, Count(*) FROM Files WHERE File='"+name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("Ошибка получения даты-времени модификации файла ") << name_file;}

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
    if(!update_lm_hash.exec()){qDebug() << QString::fromUtf8("Ошибка обновления даты-времени модификации файла и хеша ") << _file_name;}
}
//----------------------------------------------------------
void tDatabaseOp::UpdateFileInfo(const QString& _file, const QString &model_struct)
{
    //Обойти все модели в базе, по файлам модели найти файлы списка файлов
    //и по ним проверить входит ли файл в базу
    //если входит то внести дополнения в базу


    QSqlQuery all_models(db);
    all_models.prepare("SELECT DiskFile, Num FROM StructModel");
    if(!all_models.exec()){qDebug() << QString::fromUtf8("Ошибка выборки всех моделей ");}
    while(all_models.next())
    {
        QString file_model=all_models.value(0).toString();
        qlonglong num=all_models.value(1).toLongLong();

        QString model_structs=file_model.left(file_model.length()-4)+"files";
        QFile list_files(model_structs);
        if(!list_files.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << QString::fromUtf8("Не удалось открыть файл списка ") << model_structs;
            return;
        }
        while(!list_files.atEnd())
        {
            QString fp=list_files.readLine();
            QString file_path=fp;
            QString s=fp.right(1);
            if(s=="\n")
            {
                file_path=fp.left(fp.length()-1);
            }
            if(file_path==_file)
            {
                //проверить есть ли запись о таком файле в такой модели
                //если есть то обновить а если нет то создать новую запись
                QSqlQuery select_files(db);
                select_files.prepare("SELECT Count(*) FROM Files WHERE File='"+_file+"' AND Model="+QString::number(num));
                if(!select_files.exec()){qDebug() << QString::fromUtf8("Ошибка выборки файлов модели ") << file_model;}
                select_files.next();
                int count=select_files.value(0).toInt();

                    QString full_path=root+"/"+_file;
                    QFileInfo info(_file);
                    QDateTime last_mod=info.lastModified();
                    tCalcHash ch;
                    QString hash=ch.GetFileHash(full_path);
                if(count>0)
                {
                    //такой файл в модели есть
                    //стоит обновить
                    QSqlQuery new_file(db);
                    new_file.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");

                    new_file.bindValue(0, num);
                    new_file.bindValue(1, _file);
                    new_file.bindValue(2, last_mod);
                    new_file.bindValue(3, hash);
                    new_file.bindValue(4, 1);
                    new_file.bindValue(5, info.size());

                    if(!new_file.exec()){qDebug() << QString::fromUtf8("Ошибка добавления файла в модель ") << num << _file;}
                }
                else
                {
                    //такого файла в модели нет
                    //нужно добавить

                    QSqlQuery update_file(db);
                    update_file.prepare("UPDATE Files SET Model="+QString::number(num)+", LastMod='"+last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, Size="+QString::number(info.size())+" WHERE File='"+_file+"'");
                    if(!update_file.exec()){qDebug() << QString::fromUtf8("Ошибка обновления файла в модели ") << num << _file;}
                }
            }
        }
    }

//    //найти указанную модель, если таковой еще нет - создать запись
//    QSqlQuery search_model(db);
//    search_model.prepare("SELECT Count(*), Num FROM StructModels WHERE Struct='"+model_struct+"'");
//    if(!search_model.exec()){qDebug() << QString::fromUtf8("Ошибка поиска файла для обновления даты-времени модификации файла и хеша ") << _file;}
//    search_model.next();
//    int count_models=search_model.value(0).toInt();
//    qlonglong num_model=0;
//    if(count_models==0)
//    {
//        //такой модели еще нет
//        //добавить модель

//        qDebug() << "Не найдена модель, она должна быть добавлена раньше, до передачи файлов";
//    }
//    else
//    {
//        //такая модель уже есть
//        num_model=search_model.value(1).toLongLong();
//    }

//    //произвести поиск файла в базе, если найден - сравнить дату-время модификации с дисковой
//    //если не совпадает - пересчитать хеш и перезаписать данные

//    //если файла нет - пересчитать хеш и записать все в базу

//    QString relat_path=_file.right(_file.length()-root.length());
//    QSqlQuery search_file(db);
//    search_file.prepare("SELECT Count(*), LastMod FROM Files WHERE File='"+relat_path+"' AND Model="+QString::number(num_model));
//    if(!search_file.exec()){qDebug() << QString::fromUtf8("Ошибка поиска файла для обновления даты-времени модификации файла и хеша ") << _file;}
//    search_file.next();
//    int n=search_file.value(0).toInt();

////    QString full_path=root+"/"+_file;
//    QFileInfo info(_file);
//    QDateTime last_mod=info.lastModified();
//    tCalcHash ch;
//    QString hash=ch.GetFileHash(_file);

//    if(n==0)
//    {
//        //записи нет
//        //создать новую

//        QSqlQuery new_file(db);
//        new_file.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");

//        new_file.bindValue(0, num_model);
//        new_file.bindValue(1, relat_path);
//        new_file.bindValue(2, last_mod);
//        new_file.bindValue(3, hash);
//        new_file.bindValue(4, 1);
//        new_file.bindValue(5, info.size());

//        if(!new_file.exec()){qDebug() << QString::fromUtf8("Ошибка добавления файла в модель ") << num_model << relat_path;}
//    }
//    else
//    {
//        //запись есть
//        //модифицировать
//        QSqlQuery update_file(db);
//        update_file.prepare("UPDATE Files SET Model="+QString::number(num_model)+", LastMod='"+last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, Size="+QString::number(info.size())+" WHERE File='"+relat_path+"'");
//        if(!update_file.exec()){qDebug() << QString::fromUtf8("Ошибка обновления файла в модели ") << num_model << relat_path;}
//    }
}
//----------------------------------------------------------
void tDatabaseOp::DeleteingFile(const QString &_file_name)
{
    QString relat=_file_name.right(_file_name.length()-root.length());
    QSqlQuery del_file(db);
    del_file.prepare("DELETE FROM Files WHERE File='"+relat+"'");
    if(!del_file.exec()){qDebug() << QString::fromUtf8("Ошибка удаления файла ") << relat;}

}
