#include "tConveyor.h"
#include<QDebug>
#include <QMessageBox>

#include "tCalcHash.h"

extern tSettings my_settings;

tConveyor::tConveyor(Ui::MainForm *_ui, QObject *parent) :
    QObject(parent), ui(_ui), gui_comm(NULL), db_op(NULL)
{
    root=my_settings.GetRoot();
    v_conv.clear();

    gui_vf.reg("ReceiveFile",Create_tGuiReceiveFile);
    gui_vf.reg("SendFile",Create_tGuiSendFile);
    gui_vf.reg("ReceiveReport",Create_tGuiReportReceiveFile);
    gui_vf.reg("SendReport",Create_tGuiReportSendFile);
    gui_vf.reg("Error",Create_tGuiError);
    gui_vf.reg("ConnectConfirmReport",Create_tGuiConnectConfirmReport);
    gui_vf.reg("PrepareSendFile",Create_tGuiPrepareSendFile);
    gui_vf.reg("ReportPrepareSendFile",Create_tGuiReportPrepareSendFile);
    gui_vf.reg("SendAutorization",Create_tGuiSendAutorization);
    gui_vf.reg("ReportAutorization",Create_tGuiReportAutorization);
    gui_vf.reg("StartTransaction",Create_tGuiStartTransaction);
    gui_vf.reg("ReportStartTransaction",Create_tGuiReportStartTransaction);
    gui_vf.reg("CommitTransaction",Create_tGuiCommitTransaction);
    gui_vf.reg("ReportCommitTransaction",Create_tGuiReportCommitTransaction);
    gui_vf.reg("CancelTransaction",Create_tGuiCancelTransaction);
    gui_vf.reg("ReportCancelTransaction",Create_tGuiReportCancelTransaction);
    gui_vf.reg("DeleteFile",Create_tGuiDeleteFile);
    gui_vf.reg("DeleteReport",Create_tGuiReportDeleteFile);
    gui_vf.reg("GetListFiles",Create_tGuiGetListFiles);
    gui_vf.reg("ReportGetListFiles",Create_tReportGuiGetListFiles);
    gui_vf.reg("PrepareReceiveFile",Create_tGuiPrepareReceiveFile);
    gui_vf.reg("BreakReceiveFile",Create_tGuiBreakReceiveFile);
    gui_vf.reg("VerifyMoveDelete",Create_tVerifyMoveDelete);

    gui_vf.reg("GetListModels",Create_tGetListModels);
    gui_vf.reg("ReportGetListServerModels",Create_tReportGuiGetListServerModels);
}
//-----------------------------------------------------------------
tConveyor::~tConveyor()
{
//    Db.close();


    delete client_th;
    client_th=NULL;
}
//-----------------------------------------------------------------
void tConveyor::AddCommand(QByteArray _block)
{
    v_conv.push_back(_block);

}
//-----------------------------------------------------------------
void tConveyor::NextCommand()
{
    qDebug() << v_conv.size();
    if(v_conv.size()>0)
    {

        QByteArray block_comm;
        it_conv=v_conv.begin();

        block_comm=*it_conv;

        v_conv.erase(it_conv);

        OnRunGuiCommand(block_comm);


    }
    else
    {
        ClearTempFolder();
        emit EndCommands();
    }

}
//-----------------------------------------------------------------
void tConveyor::Clear()
{
    file_list.clear();
    file_list1.clear();
}
//-----------------------------------------------------------------
void tConveyor::OnCommand(QByteArray _block)
{
    QDataStream out(&_block, QIODevice::ReadOnly);

    delete gui_comm;
    gui_comm=NULL;
    QString command="";

    out >> command;

    gui_comm=gui_vf.create(command);

    connect(gui_comm, SIGNAL(RunGui(QByteArray&)), this, SLOT(OnRunGuiCommand(QByteArray&)));
    connect(gui_comm, SIGNAL(StartStop(bool)), this, SLOT(OnStartStop(bool)));
    connect(gui_comm, SIGNAL(OkAutoriz(bool)), this, SLOT(OnStart(bool)));
    connect(gui_comm, SIGNAL(NextCommand()), this, SLOT(NextCommand()));
    connect(gui_comm, SIGNAL(SendCommand(QByteArray)), client_th, SLOT(OnCommandToSocket(QByteArray)));

    gui_comm->Initialize(ui);
    gui_comm->SetLink(link);
    gui_comm->ExeCommand(out);
    delete gui_comm;
    gui_comm=NULL;
}
//-----------------------------------------------------------------
void tConveyor::OnRunGuiCommand(QByteArray& _block)
{
    QDataStream out(&_block, QIODevice::ReadWrite);

    QString command="";

    out >> command;

    qDebug() << command;

    delete gui_comm;
    gui_comm=NULL;

    gui_comm=gui_vf.create(command);

    connect(gui_comm, SIGNAL(SendCommand(QByteArray)), client_th, SLOT(OnCommandToSocket(QByteArray)));
    connect(gui_comm, SIGNAL(VerifyMoveDelete(QString&)), this, SLOT(VerifyMoveDelete(QString&)));

    gui_comm->Initialize(ui);

    gui_comm->ExeCommand(out);

    delete gui_comm;
    gui_comm=NULL;
}
//---------------------------------------------------------------------
void tConveyor::StartServer(const QString &_addr, const int _port)
{
    client_th=new tClientThread(_addr, _port);

    client_th->moveToThread(client_th);

    connect (client_th, SIGNAL(Disconnect()), this, SLOT(OnDisconnect()));
    connect(client_th, SIGNAL(SendCommandGui(QByteArray)),this , SLOT(OnCommand(QByteArray)));
    connect(this, SIGNAL(DisconnectFromServer()), client_th, SLOT(OnDisconnecting()));

    client_th->start();
}
//---------------------------------------------------------------------
void tConveyor::OnDisconnect()
{
    emit Disconnect();
}
//--------------------------------------------------------------------------------
void tConveyor::OnStartStop(const bool _res)
{
    if(_res)
    {
        emit AutorizStart();
    }
    else
    {
        emit CloseMain();
    }
}
//--------------------------------------------------------------------------------
void tConveyor::OnStart(const bool _res)
{
    if(_res)
    {
        emit SetVisible(true);
        ui->pbConnect->setEnabled(false);
        ui->pbDisconnect->setEnabled(true);
    }
    else
    {
        emit CloseMain();
    }
}
//--------------------------------------------------------------------------------
void tConveyor::AddDelCommand()
{
    QString gui_command="DeleteFile";
    QString socket_command="DeleteFile";


    for(int i=0; i<file_list1.size(); i++)
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        out << gui_command;
        out << socket_command;
        out << file_list1[i].file_name;
        out << file_list1[i].server_hash;

        AddCommand(block);
    }
}
//--------------------------------------------------------------------------------
void tConveyor::StartExecution()
{
    NextCommand();
}
//--------------------------------------------------------------------------------
void tConveyor::AddCommitTransaction(const bool _send, const QString model_struct)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    QString gui_command="CommitTransaction";

    out << gui_command;

//    QString hash="";
    int s=-1;
    if(_send)
    {
     s=1;

    }
    else
    {
     s=0;
    }
     out << s;

     out << model_struct;

     int num_files=file_list.size()+file_list1.size();

     out << num_files;

     for(int i=0; i<file_list.size(); i++)
     {
         tFileList fl;
         fl=file_list[i];

         out << fl.file_name;

         out << fl.client_hash;
         out << fl.server_hash;

     }

     //удаляемые с сервера файлы
     for(int i=0; i<file_list1.size(); i++)
     {
         tFileList fl;
         fl=file_list1[i];

         out << fl.file_name;
         out << tr("");//типа локальный хеш уже отсутствующего файла
         out << fl.server_hash;
     }
    AddCommand(block);

    if(!_send)
    {
        //Добавить команду проверки, перемещения и удаления файлов на клиентском компе
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        gui_command="VerifyMoveDelete";

        out << gui_command;
        out << model_struct;

        AddCommand(block);
    }
}
//--------------------------------------------------------------------------------
void tConveyor::AddCommitTransactionDel()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    QString gui_command="VerifyMoveDelete";

    out << gui_command;

    AddCommand(block);
}

//--------------------------------------------------------------------------------
void tConveyor::VerifyMoveDelete(QString &m_struct)
{
    model_file=m_struct;
    bool ret=true;
    temp=my_settings.GetTemp();
    root=my_settings.GetRoot();
    QDir dir(temp);
    stopped=false;
    error_file="";

    VerifyReplacedFiles();
    VerifyDeletedFiles();

    if(!stopped)
    {
        if(FolderOperation(dir, 0))
        {
            if(FolderOperation(dir, 1))
            {

                if(file_list1.size()!=0)
                {
                    bool ok=true;
                    for(int j=0; j<file_list1.size();j++)
                    {
                        //Выбирать только файлы с одной хеш-суммой
                        if(file_list1[j].server_hash=="")
                        {
                            QString file=file_list1[j].file_name;
                            ok=Delete(root+file_list1[j].file_name, error_file);
                            if(ok)
                            {
                                ret=DeleteEmptyFolders(root);
                            }
                            else
                            {
                                ret=false;;
                            }
                        }
                    }
                    if(ok)
                    {
                        ret=true;
                    }
                    else
                    {
                        ret=false;
                    }
                }
                else
                {
                    ret=true;
                }
            }
            else
            {
                ret=false;
            }
        }
        else
        {
            ret=false;
        }
    }
    else
    {
        ret=false;
    }

    if(ret)
    {
        NextCommand();
    }
    else
    {
        emit ErrorCommands();
    }
}
//---------------------------------------------------------------------------------
void tConveyor::AddStartTransaction(const bool _send)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    int num_files=file_list.size()+file_list1.size();
    if(num_files!=0)
    {

    QString gui_command="StartTransaction";

    out << gui_command;



    out << num_files;

    for(int i=0; i<file_list.size(); i++)
    {
        tFileList fl;
        fl=file_list[i];

        out << fl.file_name;
        out << fl.client_hash;
        out << fl.server_hash;
    }

    for(int i=0; i<file_list1.size(); i++)
    {
        tFileList fl;
        fl=file_list1[i];

        out << fl.file_name;
        out << fl.client_hash;
        out << fl.server_hash;
    }

    out << _send;

    AddCommand(block);
    }
}
//--------------------------------------------------------------------------------
bool tConveyor::AddSendCommand()
{

    bool ret=false;
    for(int i=0; i<file_list.size(); i++)
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

    QString gui_command="SendFile";
    QString socket_command="SendFile";

    out << gui_command;
    out << socket_command;
    out << tr("Ошибка на стороне клиента. Невозможно передать файл.");
    out << file_list[i].file_name;
    bool sending=false;
    bool receiving=false;
    QString H=db_op->GetLocalHash(file_list[i].file_name, sending, receiving);
    out << H;

    //если файла нет на диске то каков хеш и не важно, команду надо прерывать
    if(!sending)
    {
        ret=true;
        break;
    }

    AddCommand(block);
    }
    return ret;
}
//--------------------------------------------------------------------------------
bool tConveyor::SendFile(const QString &_file_name)
{
    bool ret=false;
    bool is_find=false;
    for(int i=0; i<file_list.size(); i++)
    {
        if(file_list[i].file_name==_file_name)
        {
            is_find=true;
            break;
        }
    }

    if(!is_find)
    {

    bool sending=false;
    bool receiving=false;

    tFileList fl;
    fl.file_name=_file_name;
    fl.client_hash=db_op->GetLocalHash(_file_name, sending, receiving);//"_client_hash_"+_file_name; //пока так, потом буду подставлять реальное значение
    fl.server_hash=db_op->GetServerHash(_file_name);//"_server_hash_"+_file_name; //пока так, потом буду подставлять реальное значение
    file_list << fl;

    //при отправке файла важно что бы файл был на диске
    if(!sending)
    {
        ret=true;
    }
    }
    return ret;
}
//--------------------------------------------------------------------------------
void tConveyor::AddReceiveCommand()
{
    for(int i=0; i<file_list.size(); i++)
    {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        QString gui_command="PrepareReceiveFile";
        QString socket_command="PrepareReceiveFile";

        out << gui_command;
        out << socket_command;
        out << file_list[i].file_name;

    AddCommand(block);
    }
}
//--------------------------------------------------------------------------------
bool tConveyor::ReceiveFile(const QString &_file_name)
{
    bool ret=false;
bool is_find=false;
for(int i=0; i<file_list.size(); i++)
{
    if(file_list[i].file_name==_file_name)
    {
        is_find=true;
        break;
    }
}

if(!is_find)
{
    bool sending=false;
    bool receiving=false;
tFileList fl;
fl.file_name=_file_name;
fl.client_hash=db_op->GetLocalHash(_file_name, sending, receiving);//"_client_hash_"+_file_name; //пока так, потом буду подставлять реальное значение
fl.server_hash=db_op->GetServerHash(_file_name);//"_server_hash_"+_file_name; //пока так, потом буду подставлять реальное значение
file_list << fl;

if(!receiving)
{
    ret=true;
}
}
return ret;
}

//--------------------------------------------------------------------------------
void tConveyor::OnDisconnecting()
{
    emit DisconnectFromServer();
}
//--------------------------------------------------------------------------------
bool tConveyor::DeletingFile(const QString &_file_name, const bool _send)
{
    bool sending=false;
    bool receiving=false;
    bool ret=false;
    if(_send)
    {

        bool is_find=false;

        for(int i=0; i<file_list1.size(); i++)
        {
            if(file_list1[i].file_name==_file_name)
            {
                is_find=true;
                break;
            }
        }

        if(!is_find)
        {
            QString server_hash=db_op->GetServerHash(_file_name);


        tFileList fl;
        fl.file_name=_file_name;
        fl.client_hash="";
        fl.server_hash=server_hash;
        file_list1 << fl;
        }
    }
    else
    {

        QString cl_hash=db_op->GetLocalHash(_file_name, sending, receiving);
        if(!receiving)
        {
            ret=true;
        }
        else
        {
        bool is_find=false;
        for(int i=0; i<file_list1.size(); i++)
        {
            if(file_list1[i].file_name==_file_name)
            {
                if(file_list1[i].client_hash=="" || file_list[i].server_hash=="")
                {
                    is_find=true;
                    break;
                }
            }
        }

        if(!is_find)
        {
            tFileList fl;
            fl.file_name=_file_name;
            if(_send)
            {
                fl.client_hash="";
                fl.server_hash=db_op->GetServerHash(_file_name);//"_server_hash_"+_file_name; //пока так, потом буду подставлять реальное значение
            }
            else
            {
                fl.client_hash=cl_hash;//"_client_hash_"+_file_name; //пока так, потом буду подставлять реальное значение
                fl.server_hash="";
            }
            file_list1 << fl;
        }
        }
    }
return ret;
}
//--------------------------------------------------------------------------------
void tConveyor::ClearTempFolder()
{
    QString path=my_settings.GetTemp();
    //удаление временной папки если она есть
    //делаем рекурсивный обход папок временной папки с удалением всех файлов
    //после чего удаляем всю папку
    QDir dir;
    dir.setPath(path);
    removeFolder(dir, false);

}
//--------------------------------------------------------------------------------
bool tConveyor::removeFolder(const QDir & _dir, const bool _del_dir)
{
  bool res = false;
  //Получаем список каталогов
  QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                  QDir::AllDirs |
                  QDir::NoDotAndDotDot | QDir::Hidden);
  //Получаем список файлов
  QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

  //Удаляем файлы
  foreach (QString entry, lst_files)
  {
   QString entry_abs_path = _dir.absolutePath() + "/" + entry;
   QFile::setPermissions(entry_abs_path, QFile::ReadOwner | QFile::WriteOwner);
   QFile::remove(entry_abs_path);
  }

  //Для папок делаем рекурсивный вызов
  foreach (QString entry, lst_dirs)
  {
   QString entry_abs_path = _dir.absolutePath() + "/" + entry;
   QDir dr(entry_abs_path);
   removeFolder(dr, true);
  }

  //Удаляем обрабатываемую папку
  if(_del_dir)
  {
    SetFileAttributes((LPCWSTR)(_dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
      QFile::setPermissions(_dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
    if (!QDir().rmdir(_dir.absolutePath()))
    {
      res = true;
    }
    else
    {
        res=false;
    }
  }
  else
  {
res = true;
  }
  return res;
}
//----------------------------------------------------------
bool tConveyor::FolderOperation(const QDir & _dir, const int _mode)
{
    //Получаем список каталогов
    QStringList lst_dirs = _dir.entryList(QDir::Dirs |
                                        QDir::AllDirs |
                                        QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = _dir.entryList(QDir::Files | QDir::Hidden);

    //проверяем и перемещаем файлы
    foreach (QString entry, lst_files)
    {
        if(stopped) return false;

        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QString relat_path=entry_abs_path.right(entry_abs_path.length()-my_settings.GetTemp().length());
        QString new_abs_path=root+relat_path;

        QFile::setPermissions(new_abs_path, QFile::ReadOwner | QFile::WriteOwner);

        switch (_mode)
        {
            case 0:
            {
            Verify(new_abs_path, stopped, error_file);
                break;
            }
            case 1:
            {
            Move(entry_abs_path, new_abs_path, stopped, error_file);
                break;
            }

        }
    }
    //Для папок делаем рекурсивный вызов
    foreach (QString entry, lst_dirs)
    {
        if(stopped) return false;

        QString entry_abs_path = _dir.absolutePath() + "/" + entry;
        QString new_abs_path=root+entry_abs_path.right(entry_abs_path.length()-my_settings.GetTemp().length()); //root+entry;
        QDir dir(entry_abs_path);
        if(dir.mkpath(new_abs_path))
        {
            QDir dr(entry_abs_path);
            stopped=!FolderOperation(dr, _mode);
        }
        else
        {
            error_file=new_abs_path;
            stopped=true;
            return false;
        }
    }


      return !stopped;
}
//----------------------------------------------------------
void tConveyor::Move(const QString &_entry_abs_path, const QString &_new_abs_path, bool &_stopped, QString &_error_file)
{
    QFile file_real(_new_abs_path);
    if(!file_real.exists() || file_real.remove())
    {
        QFile file_temp(_entry_abs_path);
        if(file_temp.copy(_new_abs_path))
        {
            db_op->UpdateFileInfo(_new_abs_path, model_file);

            QFile::setPermissions(_entry_abs_path, QFile::ReadOwner | QFile::WriteOwner);
            tFileList fl;
            fl.file_name=_new_abs_path;

            if(!file_temp.remove())
            {
                _error_file=file_temp.fileName();
                _stopped=true;
            }
        }
        else
        {
            _error_file=file_temp.fileName();
            _stopped=true;
        }
    }
    else
    {
        _error_file=file_real.fileName();
        _stopped=true;
    }
}
//----------------------------------------------------------
void tConveyor::Verify(const QString &_new_abs_path, bool &_stopped, QString &_error_file)
{
    QFile file_real(_new_abs_path);

    if(file_real.exists())
    {
        if(file_real.open(QIODevice::Append))
        {
            _stopped=false;
            _error_file="";
        }
        else
        {
            _stopped=true;
            _error_file=file_real.fileName();
        }
    }
    else
    {
        _stopped=false;
        _error_file="";
    }
}
//----------------------------------------------------------
bool tConveyor::Delete(const QString &_new_abs_path, QString &_error_file) const
{
    //удаляем информацию о файле из базы
    db_op->DeleteingFile(_new_abs_path);
    bool stopped=false;
    QFile file_real(_new_abs_path);
    if(file_real.exists())
    {
    stopped=!file_real.remove();
    }
    else
    {
        stopped=false;
    }
    if(stopped)
    {
        _error_file=file_real.fileName();
    }

    return !stopped;
}

//----------------------------------------------------------
bool tConveyor::DeleteEmptyFolders(const QString &_root) const
{
//    bool res = false;
    QDir dir(_root);
    //Получаем список каталогов
    QStringList lst_dirs = dir.entryList(QDir::Dirs |
                                          QDir::AllDirs |
                                          QDir::NoDotAndDotDot | QDir::Hidden);
    //Получаем список файлов
    QStringList lst_files = dir.entryList(QDir::Files | QDir::Hidden);

    if(lst_dirs.size()==0 && lst_files.size()==0)
    {
        //Если в папке нет ни файлов ни других папок то удаляем папку
        SetFileAttributes((LPCWSTR)(dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
        QFile::setPermissions(dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
        return QDir().rmdir(dir.absolutePath());

    }
    else
    {
        //Для папок делаем рекурсивный вызов
        foreach (QString entry, lst_dirs)
        {
            QString entry_abs_path = dir.absolutePath() + "/" + entry;
            DeleteEmptyFolders(entry_abs_path);

            //Если текущая папка стала пустой, удаляем ее
            QDir dir(_root);

            QStringList lst_dirs = dir.entryList(QDir::Dirs |
                                                  QDir::AllDirs |
                                                  QDir::NoDotAndDotDot | QDir::Hidden);
            //Получаем список файлов
            QStringList lst_files = dir.entryList(QDir::Files | QDir::Hidden);
            if(lst_dirs.size()==0 && lst_files.size()==0)
            {
                SetFileAttributes((LPCWSTR)(dir.path()).data(), !FILE_ATTRIBUTE_HIDDEN);
                QFile::setPermissions(dir.absolutePath(), QFile::ReadOwner | QFile::WriteOwner);
                return QDir().rmdir(dir.absolutePath());
            }
        }
        return true;
    }
}
//----------------------------------------------------------
void tConveyor::CancelOperations()
{
    file_list.clear();
    v_conv.clear();
}
//----------------------------------------------------------
void tConveyor::SetDatabaseOperator(tDatabaseOp* _db_op)
{
    db_op=_db_op;
}
//----------------------------------------------------------
void tConveyor::VerifyReplacedFiles()
{
    //проверить совпадает ли файл в рабочей папке тому что указан в базе
    //для предохранения от неожиданной потери изменений
    //внесенных между выбором файлов и началом обновления

    //обойти список file_list и сравнить времена модификации на диске и в базе
    //если они не совпадают - сверить хэши и если и они не совпадают - прервать транзакцию

    for(int i=0; i<file_list.size(); i++)
    {
        QString name=file_list[i].file_name;
        QFileInfo info(root+"/"+name);
        QDateTime disk_dt=db_op->RoundDateTime(info.lastModified());
        QDateTime base_dt=db_op->GetLastMod(name);
        if(disk_dt!=base_dt)
        {
            tCalcHash ch;
            QString disk_hash=ch.GetFileHash(root+"/"+name);
            bool s=false;
            bool r=false;
            QString base_hash=db_op->GetLocalHash(name, s, r);
            if(disk_hash!=base_hash)
            {
                stopped=true;
            }
        }
    }
}
//----------------------------------------------------------
void tConveyor::VerifyDeletedFiles()
{
    //проверить совпадает ли файл что собираемся удалять с файлом в базе
    //обойти список file_list1 и сверить с базой
    //если не совпадает по времени - сверить хеш-сумму
    //если и сумма не совпадает выставить stopped=true

    for(int i=0; i<file_list1.size(); i++)
    {
        QString name=file_list1[i].file_name;
        QFileInfo info(root+"/"+name);
        QDateTime disk_dt=db_op->RoundDateTime(info.lastModified());
        QDateTime base_dt=db_op->GetLastMod(name);
        if(disk_dt!=base_dt)
        {
            tCalcHash ch;
            QString disk_hash=ch.GetFileHash(root+"/"+name);
            bool s=false;
            bool r=false;
            QString base_hash=db_op->GetLocalHash(name, s, r);
            if(disk_hash!=base_hash && r)
            {
                stopped=true;
            }
        }
    }
}
//----------------------------------------------------------
