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

    emit SendCommand(block);
}
//************************************************************************************************
void tGuiReportReceiveFile::ExeCommand(QDataStream& _in)
{
    QString file_name="";
    qint64 file_size=-1;

    _in >> file_name;
    _in >> file_size;

    emit NextCommand();
}
//************************************************************************************************
void tGuiReportSendFile::ExeCommand(QDataStream& _in)
{
    QString file_name="";
    qint64 file_size=-1;

    _in >> file_name;
    _in >> file_size;

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

    //Отмена транзакции из-за ошибки
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    block.clear();

    QString command="CancelTransaction";

    out << command;
    emit SendCommand(block);

/*
         QTextCodec *codec =QTextCodec::codecForName("Windows-1251");
//        QTextCodec *codec =QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForTr(codec);
        QTextCodec::setCodecForCStrings(codec);

    QString before="\\";
    QString after="/";
    QString ret=codec->toUnicode(_path.replace(before, after).toAscii());
    */

   QTextCodec *codec =QTextCodec::codecForName("Windows-1251");

   QTextCodec::setCodecForTr(codec);
   QTextCodec::setCodecForCStrings(codec);

    QMessageBox MB;
    QString S=QString::fromUtf8("Ошибка в команде ")+QString::number(num_error)+"\n"+client_detail;
    QString S1=codec->toUnicode(S.toAscii())+"\n"+detail;
    MB.setText(S1);
    MB.setWindowTitle(error);
    MB.exec();

}
//************************************************************************************************
void tGuiConnectConfirmReport::ExeCommand(QDataStream& _in)
{
    bool ok=false;

    _in >> ok;

    if(!ok)
    {
        QMessageBox mb;
        mb.setText(QString::fromUtf8("В подключении отказано"));
        mb.setWindowTitle(QString::fromUtf8("Подключение"));
        mb.exec();
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

    emit SendCommand(block);


}
//************************************************************************************************
void tGuiReportAutorization::ExeCommand(QDataStream &_in)
{
    bool ret=false;

    _in >> ret;

    if(ret)
    {
        //продолжить работу
        emit OkAutoriz(true);
    }
    else
    {
        //завершить работу
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
    emit NextCommand();
}
//************************************************************************************************
void tGuiCommitTransaction::ExeCommand(QDataStream &_in)
{
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


    emit SendCommand(block);
}
//************************************************************************************************
void tGuiReportCancelTransaction::ExeCommand(QDataStream &)
{
    ((MainForm*)link)->CancelAllOperations();
    QMessageBox mb;
    mb.setText(QString::fromUtf8("Транзакция отменена"));
    mb.setWindowTitle(QString::fromUtf8("Transaction"));
    mb.exec();
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

    emit SendCommand(block);
}
//************************************************************************************************
void tGuiReportDeleteFile::ExeCommand(QDataStream &_in)
{
    QString file_name="";

    _in >> file_name;

    emit NextCommand();
}
//************************************************************************************************
void tGuiGetListFiles::ExeCommand(QDataStream &_in)
{
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

    emit SendCommand(block);
}
//************************************************************************************************
void tGuiBreakReceiveFile::ExeCommand(QDataStream &)
{
    QMessageBox mb;
    mb.setText(QString::fromUtf8("Транзакция отменена\nОдин из локальных файлов нельзя открыть для записи"));
    mb.setWindowTitle(QString::fromUtf8("Break transaction"));
    mb.exec();
}
//************************************************************************************************
void tVerifyMoveDelete::ExeCommand(QDataStream &_out)
{
    QString m_struct="";
    _out >> m_struct;
    emit VerifyMoveDelete(m_struct);
}
//************************************************************************************************
void tGetListModels::ExeCommand(QDataStream &_in)
{
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
    _in.device()->seek(0);
    _in.device()->seek(54);


    QByteArray block;
    block=_in.device()->readAll();


    ((MainForm*)link)->SaveServerModelFiles(block);

    bool tr=((MainForm*)link)->GetIsTransaction();
    if(tr)
    {
    //Сюда процедуру обновления Last таблиц с серверных данных (true)
    ((MainForm*)link)->CorrectLastSynch(true);
    emit FinalBlockTransactions();
    }

}
//************************************************************************************************
void tUpdateMainLocal::ExeCommand(QDataStream &)
{
    ((MainForm*)link)->OnListFilesLocal();
    ((MainForm*)link)->CorrectLastSynch(false);
    emit NextCommand();

}
//************************************************************************************************
