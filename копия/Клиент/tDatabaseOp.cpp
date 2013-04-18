#include "tDatabaseOp.h"
#include "tSettings.h"
#include <QMessageBox>
#include "tCalcHash.h"

extern tSettings my_settings;
//----------------------------------------------------------------------
tDatabaseOp::tDatabaseOp()
{
    root=my_settings.GetRoot();
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Database.db");
    if(!db.open()){qDebug() << db.lastError().text();}
}
//----------------------------------------------------------------------
tDatabaseOp::~tDatabaseOp()
{
    db.close();
}
//----------------------------------------------------------------------
void tDatabaseOp::RefreshModelsFiles()
{
    QString root=my_settings.GetRoot();

    //Начало работы с базой данных
    //Начало транзакции
    bool b=db.transaction();

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
    b=db.commit();

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

                    QDateTime list_files_date_time=round_dt;

                    QString summ_list_hash="";

                    UpdateModelFiles(num_model, _folder, entry, summ_list_hash);

                    //обновить запись в таблице моделей по дате модификации, хеш-сумме и общей хеш-сумме

                    QSqlQuery update_struct_models(db);

                    QString dt=list_files_date_time.toString(Qt::ISODate);

                    QString sql="UPDATE StructModels SET ListFilesLastMod='"+dt+"', ListFilesHash='"+list_files_hash+"', SummListHash='"+summ_list_hash+"', Found=1 WHERE DiskFile='"+NormalizePathFiles(relat+"/"+entry)+"'";
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

            if(!files_model.exec()){qDebug() << QString::fromUtf8("Ошибка при записи файла списка ") << root+file_path;}
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

    tCalcHash ch_model;
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

                        update_dt_hash.prepare("UPDATE Files SET LastMod='"+disk_last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, "+QString::number(info.size())+" WHERE File='"+relat+"/"+entry+"' AND Model="+QString::number(_num));
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
void tDatabaseOp::GetListModels(QStringList &_list)
{
    _list.clear();
    QSqlQuery list_struct_models(db);
    list_struct_models.prepare("SELECT Struct FROM StructModels");
    if(!list_struct_models.exec()){qDebug() << QString::fromUtf8("Ошибка выборки структуры моделей ");}
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
    if(!list_model_files.exec()){qDebug() << QString::fromUtf8("Ошибка выборки файлов модели ") << _str;}
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
    QSqlQuery del_all_server_models(db);
    del_all_server_models.prepare("DELETE FROM ServerStructModels");
    if(!del_all_server_models.exec()){qDebug() << QString::fromUtf8("Ошибка удаления всех серверных моделей ");}

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

//        files_model.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Del, Size)"
//                            "VALUES(?, ?, ?, ?, ?, ?, ?);");

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

        if(!insert_server_model.exec()){qDebug() << QString::fromUtf8("Ошибка добавления серверной модели ") << DiskFile;}
        qlonglong num_server_model=insert_server_model.lastInsertId().toLongLong();

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
            if(!insert_server_files.exec()){qDebug() << QString::fromUtf8("Ошибка добавления файлов серверной модели ") << num_server_model;}
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
    if(!models.exec()){qDebug() << QString::fromUtf8("Ошибка составления списка серверных моделей ");}
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
    if(!models.exec()){qDebug() << QString::fromUtf8("Ошибка составления списка серверных файлов ");}
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
    if(!file.exec()){qDebug() << QString::fromUtf8("Ошибка получения хеша локального файла ") << _name_file;}
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
            }
//            else
//            {
//                //времена совпадают, можно использовать хеш из базы
//                //он уже получен
//            }
        }
        else
        {

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
//        else
//        {
//            //нет ни в базе ни на диске
//            //нечего вычислять
//        }
    }
    _sending = existing_base && existing_disk;

    _receiving = existing_base && base_date_time==disk_date_time || !existing_disk;

//    QDateTime disk_date_time=info.lastModified();
//    QDateTime round=RoundDateTime(disk_date_time);
//    bool _existing_disk=info.exists();

//    QSqlQuery file(db);
//    file.prepare("SELECT Hash, LastMod, Count(*) FROM Files WHERE File='"+_name_file+"'");
//    if(!file.exec()){qDebug() << QString::fromUtf8("Ошибка получения хеша локального файла ") << _name_file;}
//    file.next();

//    bool existing_base=file.value(2).toInt()>0;
//    if(existing_base && _existing_disk)
//    {
//    QDateTime base_date_time=file.value(1).toDateTime();
//    if(round==base_date_time)
//    {
//    //дата-время модификации совпадают, можно использовать данные из базы
//    return file.value(0).toString();
//    }
//    else
//    {
//    //дата-время не совпадают, нужно передать пустую строку хеша как сигнал о том что нужно превать работу


//    tCalcHash ch;
//    hash=ch.GetFileHash(root+_name_file);



////        QSqlQuery update_file(db);
////        update_file.prepare("UPDATE Files SET LastMod='"+disk_date_time.toString(Qt::ISODate)+"', Hash='"+hash+"' WHERE File='"+name_file+"'");
////        if(!update_file.exec()){qDebug() << QString::fromUtf8("Ошибка обновления даты-времени и хеша локального файла ") << name_file;}



//    }

//    }

    return hash;
}
//----------------------------------------------------------
QString tDatabaseOp::GetServerHash(const QString& name_file)
{
    QSqlQuery file(db);
    file.prepare("SELECT Hash, Count(*) FROM ServerFiles WHERE File='"+name_file+"'");
    if(!file.exec()){qDebug() << QString::fromUtf8("Ошибка получения хеша серверного файла ") << name_file;}

    file.next();
//    int N=file.value(1).toInt();
    return file.value(0).toString();
}
//----------------------------------------------------------
void tDatabaseOp::UpdateFileInfo(const QString& _file, const QString &model_struct)
{
    //найти указанную модель, если таковой еще нет - создать запись
    QSqlQuery search_model(db);
    search_model.prepare("SELECT Count(*), Num FROM StructModels WHERE Struct='"+model_struct+"'");
    if(!search_model.exec()){qDebug() << QString::fromUtf8("Ошибка поиска файла для обновления даты-времени модификации файла и хеша ") << _file;}
    search_model.next();
    int count_models=search_model.value(0).toInt();
    qlonglong num_model=0;
    if(count_models==0)
    {
        //такой модели еще нет
        //добавить модель

        qDebug() << "Не найдена модель, она должна быть добавлена раньше, до передачи файлов";
    }
    else
    {
        //такая модель уже есть
        num_model=search_model.value(1).toLongLong();
    }

    //произвести поиск файла в базе, если найден - сравнить дату-время модификации с дисковой
    //если не совпадает - пересчитать хеш и перезаписать данные

    //если файла нет - пересчитать хеш и записать все в базу

    QString relat_path=_file.right(_file.length()-root.length());
    QSqlQuery search_file(db);
    search_file.prepare("SELECT Count(*), LastMod FROM Files WHERE File='"+relat_path+"'");
    if(!search_file.exec()){qDebug() << QString::fromUtf8("Ошибка поиска файла для обновления даты-времени модификации файла и хеша ") << _file;}
    search_file.next();
    int n=search_file.value(0).toInt();

//    QString full_path=root+"/"+_file;
    QFileInfo info(_file);
    QDateTime last_mod=info.lastModified();
    tCalcHash ch;
    QString hash=ch.GetFileHash(_file);

    if(n==0)
    {
        //записи нет
        //создать новую

        QSqlQuery new_file(db);
        new_file.prepare("INSERT INTO Files (Model, File, LastMod, Hash, Found, Size) VALUES (?, ?, ?, ?, ?, ?)");

        new_file.bindValue(0, num_model);
        new_file.bindValue(1, relat_path);
        new_file.bindValue(2, last_mod);
        new_file.bindValue(3, hash);
        new_file.bindValue(4, 1);
        new_file.bindValue(5, info.size());

        if(!new_file.exec()){qDebug() << QString::fromUtf8("Ошибка добавления файла в модель ") << num_model << relat_path;}
    }
    else
    {
        //запись есть
        //модифицировать
        QSqlQuery update_file(db);
        update_file.prepare("UPDATE Files SET Model="+QString::number(num_model)+", LastMod='"+last_mod.toString(Qt::ISODate)+"', Hash='"+hash+"', Found=1, Size="+QString::number(info.size())+" WHERE File='"+relat_path+"'");
        if(!update_file.exec()){qDebug() << QString::fromUtf8("Ошибка обновления файла в модели ") << num_model << relat_path;}
    }
}
//----------------------------------------------------------
void tDatabaseOp::DeleteingFile(const QString &_file_name)
{
    QString relat=_file_name.right(_file_name.length()-root.length());
    QSqlQuery del_file(db);
    del_file.prepare("DELETE FROM Files WHERE File='"+relat+"'");
    if(!del_file.exec()){qDebug() << QString::fromUtf8("Ошибка удаления файла ") << relat;}

}
//----------------------------------------------------------
QDateTime tDatabaseOp::GetLastMod(const QString& _file_name)
{
    QSqlQuery sel_last_dt(db);
    sel_last_dt.prepare("SELECT Count(*), LastMod FROM Files WHERE File='"+_file_name+"'");
    if(!sel_last_dt.exec()){qDebug() << QString::fromUtf8("Ошибка выборки даты-времени модификации файла ") << _file_name;}
    sel_last_dt.next();
    QDateTime dt=sel_last_dt.value(1).toDateTime();
    return dt;
}
