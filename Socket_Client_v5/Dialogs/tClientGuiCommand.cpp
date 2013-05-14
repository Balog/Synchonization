#include "tClientGuiCommand.h"
#include "MainForm.h"
#include "ui_MainForm.h"
#include <QMessageBox>

extern tSettings my_settings;
void tGuiReceiveFile::ExeCommand(QDataStream& _in)
{
    QString command="";
    QString client_detail="";
    _in >> command;
    _in >> client_detail;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();
    QString file_name="";

    _in >> file_name;

    int num_err=1;
    QString error="Unable to open file";
    QString detail="";//Root: "+ui->leRoot->text()+"\n File: "+file_name;

    out << command;
    out << file_name;
    out << num_err;
    out << error;
    out << detail;
    out << client_detail;

    l="tClientGuiCommand \tGuiReceiveFile\tКоманда приема файла "+file_name.toUtf8();
    log.Write(l);

    emit SendCommand(block);
}
//************************************************************************************************
void tGuiSendFile::ExeCommand(QDataStream& _in)
{
    QString command="";
    QString client_detail="";

    _in >> command;
    _in >> client_detail;

    QString file_name="";
    _in >> file_name;

    QString Hash="";
    _in >> Hash;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    int num_err=1;
    QString error="Unable to open file";
    QString detail="";//Root: "+ui->leRoot->text()+"\n File: "+file_name;


    out << command;
    out << file_name;
    out << num_err;
    out << error;
    out << detail;
    out << client_detail;
    out << Hash;

    l="tClientGuiCommand \tGuiSendFile\tКоманда передачи файла "+file_name.toUtf8();
    log.Write(l);

    emit SendCommand(block);
}
//************************************************************************************************
void tGuiReportReceiveFile::ExeCommand(QDataStream& _in)
{
    QString file_name="";
    qint64 file_size=-1;

    _in >> file_name;
    _in >> file_size;

    l="tClientGuiCommand \tGuiReportReceiveFile\tОтчет о приеме файла "+file_name.toUtf8();
    log.Write(l);

    emit NextCommand();
}
//************************************************************************************************
void tGuiReportSendFile::ExeCommand(QDataStream& _in)
{
    QString file_name="";
    qint64 file_size=-1;

    _in >> file_name;
    _in >> file_size;

    l="tClientGuiCommand \tGuiReportSendFile\tОтчет о передаче файла "+file_name.toUtf8();
    log.Write(l);

    emit NextCommand();
}
//************************************************************************************************
void tGuiError::ExeCommand(QDataStream& _in)
{
    int num_error=-9999;
    QString error="";
    QString detail="";
    QString client_detail="";

    _in >> num_error;
    _in >> error;
    _in >> detail;
    _in >> client_detail;

    if(((MainForm*)link)->GetIsTransaction())
    {
        //Отмена транзакции из-за ошибки
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        block.clear();

        QString command="CancelTransaction";

        out << command;

        qDebug() << QString::fromUtf8("Ошибка ") << num_error << " " << detail << " " << client_detail;

        l="tClientGuiCommand \tGuiError\tОшибка на сервере "+detail+" "+client_detail;
        log.Write(l);

        l="tClientGuiCommand \tGuiError\tКоманда серверу на отмену транзакции ";
        log.Write(l);

        emit SendCommand(block);
    }
    else
    {
        QTextCodec *codec =QTextCodec::codecForName("Windows-1251");

        QTextCodec::setCodecForTr(codec);
        QTextCodec::setCodecForCStrings(codec);
        QMessageBox MB;
        //        QString S=QString::fromUtf8("Ошибка в команде ")+QString::number(num_error)+"\n"+client_detail;
        //        QString S1=codec->toUnicode(S.toAscii())+"\n"+detail;

        QString text="";
        QString title="";
        switch(num_error)
        {
        case 0:
        {
            title=QString::fromUtf8("Авторизация");
            text=QString::fromUtf8("Авторизация не удалась");

            MB.setText(text);
            MB.setWindowTitle(title);
            MB.exec();
            emit OkAutoriz(false);

            break;
        }
        case 12:
        {
            title=QString::fromUtf8("Запись логина");
            text=QString::fromUtf8(detail.toAscii());

            MB.setText(text);
            MB.setWindowTitle(title);
            MB.exec();
//            emit OkAutoriz(false);
            ((MainForm*)link)->UpdateLogins();
            break;
        }
        }

    }

}
//************************************************************************************************
void tGuiConnectConfirmReport::ExeCommand(QDataStream& _in)
{
    bool ok=false;

    _in >> ok;

    if(!ok)
    {
        l="tClientGuiCommand \tGuiConnectConfirmReport\tВ подключении отказано";
        log.Write(l);

        QMessageBox mb;
        mb.setText(QString::fromUtf8("В подключении отказано"));
        mb.setWindowTitle(QString::fromUtf8("Подключение"));
        mb.exec();
    }
    else
    {
        l="tClientGuiCommand \tGuiConnectConfirmReport\tПодключение разрешено";
        log.Write(l);
    }

    emit StartStop(ok);
}
//************************************************************************************************
void tGuiPrepareSendFile::ExeCommand(QDataStream &_in)
{
    QString file_name="";
    QString command="";

    _in >> command;
    _in >> file_name;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();


    out << command;
    out << file_name;

    l="tClientGuiCommand \tGuiPrepareSendFile\tПодготовка к передаче файла "+file_name.toUtf8();
    log.Write(l);

    emit SendCommand(block);


}
//************************************************************************************************
void tGuiReportPrepareSendFile::ExeCommand(QDataStream &_in)
{
    QString file_name="";

    _in >> file_name;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("SendFile");
    out << tr("SendFile");
    out << tr("Ошибка на стороне клиента. Невозможно передать файл.");
    out << file_name;

    l="tClientGuiCommand \tGuiReportPrepareSendFile\tОтчет о подготовке к передаче файла "+file_name.toUtf8();
    log.Write(l);

    emit RunGui(block);


}
//************************************************************************************************
void tGuiSendAutorization::ExeCommand(QDataStream &_in)
{
    QString login="";
    QString password="";
    QString command="";

    _in >> command;
    _in >> login;
    _in >> password;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << command;
    out << login;
    out << password;

    l="tClientGuiCommand \tGuiSendAutorization\tОтправка логина и пароля Логин: "+login.toUtf8();
    log.Write(l);

    emit SendCommand(block);



}
//************************************************************************************************
void tGuiReportAutorization::ExeCommand(QDataStream &_in)
{
    bool ret=false;

    _in >> ret;

    if(ret)
    {
        l="tClientGuiCommand \tGuiReportAutorization\tОтчет об авторизации. Авторизация принята ";
        log.Write(l);

        //продолжить работу
        emit OkAutoriz(true);
    }
    else
    {
        //завершить работу

        l="tClientGuiCommand \tGuiReportAutorization\tОтчет об авторизации. В авторизации отказано ";
        log.Write(l);

        QMessageBox mb;
        mb.setText(QString::fromUtf8("Неверный логин или пароль\nВ авторизации отказано"));
        mb.setWindowTitle(QString::fromUtf8("Авторизация"));
        mb.exec();

        emit OkAutoriz(false);
    }
}
//************************************************************************************************
void tGuiStartTransaction::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGuiStartTransaction\tНАЧАЛО ТРАНЗАКЦИИ ";
    log.Write(l);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("StartTransaction");

    int num_files=-1;
    _in >> num_files;

    out << num_files;

    for(int i=0; i<num_files; i++)
    {
        QString file_name="";
        QString client_hash="";
        QString server_hash="";

        _in >> file_name;
        _in >> client_hash;
        _in >> server_hash;

        out << file_name;
        out << client_hash;
        out << server_hash;
    }

    bool send=false;

    _in >> send;
    out << send;

    emit SendCommand(block);
}
//************************************************************************************************
void tGuiReportStartTransaction::ExeCommand(QDataStream &)
{
    l="tClientGuiCommand \tGuiReportStartTransaction\tОтчет о начале транзакции ";
    log.Write(l);

    emit NextCommand();
}
//************************************************************************************************
void tGuiCommitTransaction::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGuiCommitTransaction\tКоммит транзакции ";
    log.Write(l);

    int send=-1;
    _in >> send;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << tr("CommitTransaction");
    out << send;

    QString model_file="";

    //    _in >> model_file;
    //    out << model_file;

    int num_files=-1;
    _in >> num_files;

    out << num_files;

    for(int i=0; i<num_files; i++)
    {
        QString file_name="";
        _in >> file_name;
        out << file_name;

        QString loc_hash="";
        _in >> loc_hash;
        out << loc_hash;

        QString serv_hash="";
        _in >> serv_hash;
        out << serv_hash;

    }


    emit SendCommand(block);
}
//************************************************************************************************
void tGuiReportCommitTransaction::ExeCommand(QDataStream &)
{
    l="tClientGuiCommand \tGuiCommitTransaction\tотчет о коммите транзакции ";
    log.Write(l);

    //для синхронизации последнего состояния нужно использовать разные процедуры при отправке и при приеме файлов
    //И при этом проводить ее нужно после того как состояние докальных и серверных таблиц будет обновлено
    //Значит делать это нужно в конце всех транзакций, после обновления локальныз таблиц (при приеме файлов и локальном удалении)
    //или после обновления серверных таблиц (при передаче и серверном удалении)
    //    ((MainForm*)link)->CorrectLastSynch();
    emit NextCommand();
}
//************************************************************************************************
void tGuiCancelTransaction::ExeCommand(QDataStream &)
{
    //Отмена транзакции из ГУИ
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("CancelTransaction");
    qDebug() << QString::fromUtf8("отмена транзакции из ГУИ");
    l="tClientGuiCommand \tGuiCancelTransaction\tОтмена транзакции из GUI";
    log.Write(l);

    emit SendCommand(block);


}
//************************************************************************************************
void tGuiReportCancelTransaction::ExeCommand(QDataStream &)
{
    l="tClientGuiCommand \tGuiReportCancelTransaction\tОтчет об отмене транзакции с сервера";
    log.Write(l);

    ((MainForm*)link)->CancelAllOperations();

    qDebug() << QString::fromUtf8("Отмена транзакции с сервера ");

    emit EndConveyor();
}
//************************************************************************************************
void tGuiDeleteFile::ExeCommand(QDataStream &_in)
{
    QString command="";
    QString client_detail="";

    _in >> command;
    QString file_name="";
    _in >> file_name;
    QString hash="";
    _in >> hash;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    int num_err=1;
    QString error="Unable to delete file";
    QString detail="";//Root: "+ui->leRoot->text()+"\n File: "+file_name;


    out << command;
    out << file_name;
    out << hash;
    out << num_err;
    out << error;
    out << detail;
    out << client_detail;

    l="tClientGuiCommand \tGuiDeleteFile\tУдаление файла на сервере "+file_name.toUtf8();
    log.Write(l);

    emit SendCommand(block);


}
//************************************************************************************************
void tGuiReportDeleteFile::ExeCommand(QDataStream &_in)
{
    QString file_name="";

    _in >> file_name;

    QString l="tClientGuiCommand \tGuiReportDeleteFile\tОтчет об удалении файла на сервере ";
    //    l=l1.toUtf8()+file_name.toUtf8();
    log.Write(l);

    emit NextCommand();


}
//************************************************************************************************
void tGuiGetListFiles::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGuiGetListFiles\tЗапрос списка файлов с сервера ";
    log.Write(l);

    QString comm="";

    _in >> comm;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << comm;

    emit SendCommand(block);
}
//************************************************************************************************
void tReportGuiGetListFiles::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tReportGuiGetListFiles\tОтчет запроса списка файлов с сервера ";
    log.Write(l);
    //    slm_list=NULL;
    QStringList list;

    int num=-1;
    _in >> num;
    for(int i=0; i<num;i++)
    {
        QString file="";
        _in >> file;
        list.push_back(my_settings.GetRoot()+file);
    }
    //(tClient*)link)
    ((MainForm*)link)->LocalListFile(list);

}
//************************************************************************************************
void tGuiPrepareReceiveFile::ExeCommand(QDataStream &_in)
{
    QString comm="";
    QString file_name="";

    _in >> comm;
    _in >> file_name;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << comm;
    out << file_name;

    l="tClientGuiCommand \tGuiPrepareReceiveFile\tПодготовка к приему файла ";
    l=l+file_name.toUtf8();
    log.Write(l);


    emit SendCommand(block);


}
//************************************************************************************************
void tGuiBreakReceiveFile::ExeCommand(QDataStream &)
{
    l="tClientGuiCommand \tGuiBreakReceiveFile\t Транзакция отменена. Один из локальных файлов нельзя открыть для записи";
    log.Write(l);

    qDebug() << QString::fromUtf8("Транзакция отменена\nОдин из локальных файлов нельзя открыть для записи");
    emit EndConveyor();
}
//************************************************************************************************
void tVerifyMoveDelete::ExeCommand(QDataStream &_out)
{
    l="tClientGuiCommand \tVerifyMoveDelete\t Команда проверки, замены и удаления локальных файлов";
    log.Write(l);

    QString m_struct="";
    _out >> m_struct;
    emit VerifyMoveDelete(m_struct);
}
//************************************************************************************************
void tGetListModels::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGetListModels\t Запрос списка моделей с сервера";
    log.Write(l);

    QString comm="";

    _in >> comm;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << comm;

    emit SendCommand(block);
}
//************************************************************************************************
void tReportGuiGetListServerModels::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGetListModels\t Отчет запроса списка моделей с сервера";
    log.Write(l);

    _in.device()->seek(0);
    _in.device()->seek(54);


    QByteArray block;
    block=_in.device()->readAll();


    ((MainForm*)link)->SaveServerModelFiles(block);
    if(!((MainForm*)link)->IsRequeryServerModel)
    {
    bool tr=((MainForm*)link)->GetIsTransaction();
    if(tr)
    {
        //СЮДА ОКОНЧАНИЕ ПРОЦЕДУР ОБНОВЛЕНИЯ ТАБЛИЦ LAST
        //НАЧАЛО В void tModelsConveyor::StartSendDeleteFiles()
        l="tClientGuiCommand \tGetListModels\t ОКОНЧАНИЕ ПРОЦЕДУР ОБНОВЛЕНИЯ ТАБЛИЦ LAST";
        log.Write(l);

        ((MainForm*)link)->CorrectLastSynch(true);
        ((MainForm*)link)->IsRequeryServerModel=false;

        emit FinalBlockTransactions();
    }
    else
    {
        //построение дерева вызвать позже
        //сначала нужно обновить таблицу разрешений

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);


        out << QString("ReceiveReadPermissions");
        out << QString("ReceiveReadPermissions");

        l="tClientGuiCommand \tGuiReportPrepareSendFile\tЗапрос таблицы разрешений на чтение моделей ";
        log.Write(l);

        emit RunGui(block);
//        ((MainForm*)link)->TreesBuildings();
    }
    }

}
//************************************************************************************************
void tUpdateMainLocal::ExeCommand(QDataStream &)
{
    l="tClientGuiCommand \tUpdateMainLocal\t Обновление локальных и Last таблиц ";
    log.Write(l);

    ((MainForm*)link)->OnListFilesLocal();
    emit NextCommand();

}
//************************************************************************************************
void tGUISaveLoginPassword::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \ttSaveLoginPassword\t Передача данных на сервер о пользователе для регистрации ";
    log.Write(l);

    QString comm="";
    QString login="";
    QString password="";
    bool new_user=false;
    qlonglong num_login=0;
    int row=-1;

    _in >> comm;
    _in >> login;
    _in >> password;
    _in >> new_user;
    _in >> num_login;
    _in >> row;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << comm;
    out << login;
    out << password;
    out << new_user;
    out << num_login;
    out << row;

    emit SendCommand(block);

}
//************************************************************************************************
void tGUIReportSaveLoginPassword::ExeCommand(QDataStream &_in)
{
qlonglong s_num=0;
_in >> s_num;
((MainForm*)link)->RegisterUser(s_num);
}
//************************************************************************************************
void tGUIDeleteLogin::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \ttGUIDeleteLogin\t Передача команды удаления пользователя ";
    log.Write(l);

    QString comm;
    qlonglong num_log=0;
    _in >> comm;
    _in >> num_log;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << comm;
    out << num_log;

    emit SendCommand(block);
}
//************************************************************************************************
void tGUIReportDeleteLogin::ExeCommand(QDataStream &_in)
{
qlonglong s_num=0;
_in >> s_num;
((MainForm*)link)->DeleteUser(s_num);
}
//************************************************************************************************
void tGUIReceiveLoginsTable::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \ttGUIGetLoginsTable\t Передача команды запроса таблицы логинов ";
    log.Write(l);

    QString comm="";
    _in >> comm;


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << comm;

    emit SendCommand(block);
}
//************************************************************************************************
void tGUIReportReceiveLoginsTable::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGetListModels\t Отчет запроса списка моделей с сервера";
    log.Write(l);

    _in.device()->seek(0);
    _in.device()->seek(62);


    QByteArray block;
    block=_in.device()->readAll();


    ((MainForm*)link)->UpdateLoginsTable(block);

}
//************************************************************************************************
void tGUIReceiveReadPermissions::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \ttGUIReceiveReadPermissions\t Передача команды запроса таблицы разрешений ";
    log.Write(l);

    QString comm="";
    _in >> comm;

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    out << comm;

    emit SendCommand(block);
}
//************************************************************************************************
void tGUIReportReceiveReadPermissions::ExeCommand(QDataStream &_in)
{
    l="tClientGuiCommand \tGetListModels\t Отчет запроса списка разрешений на чтение моделей с сервера";
    log.Write(l);

    _in.device()->seek(0);
    _in.device()->seek(70);


    QByteArray block;
    block=_in.device()->readAll();


    ((MainForm*)link)->UpdateModelRead(block);

}
//************************************************************************************************
