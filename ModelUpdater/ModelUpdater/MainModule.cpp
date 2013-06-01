#include "MainModule.h"
#include<QMessageBox>
#include<tSettings.h>

extern tSettings my_settings;

MainModule::MainModule(QObject *parent) :
    QObject(parent), db_op(new tDatabaseOp)
{
    mod_conv= new tModelsConveyor(this, db_op);

    mod_conv->StartServer(my_settings.GetServerAddr(), my_settings.GetServerPort());
}
//---------------------------------------------------------
bool MainModule::GetIsTransaction()
{
    return mod_conv->GetIsTransaction();
}
//---------------------------------------------------------
void MainModule::ViewError(const int _num_error, const QString& _error, const QString &_detail, const QString& _client_detail)
{
    //Вообще будет выводиться куда-то на панель а пока так
    QMessageBox MB;
    MB.setText(_error+"\n"+_detail+"\n"+_client_detail);
    MB.setWindowTitle(QString::fromUtf8("Ошибка ")+QString::number(_num_error));
    MB.exec();
}
//---------------------------------------------------------
void MainModule::UpdateLogins()
{
    emit Update_Logins();
//    db_op->GetLoginsModel(tableModel);

//    ui->lvLogins->setModel(tableModel);
}
//---------------------------------------------------------
bool MainModule::VerifyUserFolders()
{
    QString message="";
    QString project_folder="";
    QString temp_folder="";
    if(!modify_folder && db_op->VerifyUserFolders(_user_login, project_folder, temp_folder, message))
    {

        return true;
    }
    else
    {
        emit ErrorUserFolders(db_op, _user_login, message, true);
//        form_new_path->SetDatabase(db_op);
//        form_new_path->SetLogin(_user_login);
//        form_new_path->SetMessage(message);
//        form_new_path->setVisible(true);
        return false;
    }

}
//----------------------------------------------------------
void MainModule::OnContinueStart()
{
    db_op->SaveFoldersToSettings(_user_login);

    //Это все можно сделать из главной формы по окончании старта
//    ui->leRoot->setText(my_settings.GetRoot());
//    ui->leTemp->setText(my_settings.GetTemp());
    bool is_admin_user=false;
    bool is_writable_user=false;
    db_op->GetPermissionsUser(_user_login, is_admin_user, is_writable_user);

    //Это можно сделать по окончании старта, проверить переменные и все сделать в главной форме
//    if(!is_admin_user)
//    {
//        ui->tabWidget->setTabEnabled(2, false);
//    }
//    else
//    {
//        ui->tabWidget->setTabEnabled(2, true);
//    }

//    if(!is_writable_user)
//    {
//        ui->tabWidget->setTabEnabled(1, false);
//    }
//    else
//    {
//        ui->tabWidget->setTabEnabled(1, true);
//    }
//    ui->tabWidget->setCurrentIndex(0);

    db_op->RefreshModelsFiles();

    OnListFilesLocal();
    OnListFiles();

}
//----------------------------------------------------------
void MainModule::CancelAllOperations()
{
    OnClearSendAll();
    OnClearDelete();
    OnReceiveClear();
    OnClearDeleteLocal();

    mod_conv->CancelOperations();
}
//----------------------------------------------------------
void MainModule::OnListFilesLocal()
{
    db_op->RefreshModelsFiles();

    VerifyLastTable(_user_login);
}
//---------------------------------------------------------------------
void MainModule::OnListFiles()
{
    tLog log;
    log.Write(tr("MainForm \t OnListFiles \t Запрос с главной формы GetListModels через RunGui"));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << tr("GetListModels");
    out << tr("GetListModels");

    emit RunGui(block);
}
//---------------------------------------------------------------------
void MainModule::OnClearSendAll()
{
    listSend.clear();
}
//---------------------------------------------------------------------
void MainModule::OnClearDelete()
{
    listDel.clear();
}
//---------------------------------------------------------------------
void MainModule::OnReceiveClear()
{
    listRec.clear();
}
//---------------------------------------------------------------------
void MainModule::OnClearDeleteLocal()
{
    listDelLoc.clear();

}
//---------------------------------------------------------------------
void MainModule::VerifyLastTable(const QString& _user_login)
{
    //Проверить если есть такие модели в таблице Last
    //которых уже нет ни в серверной ни в локальной папке
    db_op->VerifyLastTable(_user_login);

}
//----------------------------------------------------------
void MainModule::SaveServerModelFiles(QByteArray &_block)
{
//    //распарсить переданый блок и записать в папки серверного состояния моделей и их файлов
    db_op->SaveServerModelFiles(_block);
}
//----------------------------------------------------------
void MainModule::CorrectLastSynch(const bool _server)
{

    mod_conv->MarkLastTables(_server, _user_login);

    db_op->ExecUpdateLastSynch(_server, _user_login);
    mod_conv->ClearAllList();
}
//----------------------------------------------------------
void MainModule::EndUpdateServerModel(const bool _rebuild)
{
    //формирование дерева чтения по полученым и имеющимся данным после обновления данных с сервера

    OnListFilesLocal();

    BuildingTree(_user_login);

    emit EndUpdatingFromServer(list_compare, _rebuild);
    //Это все переместить в форму, там ему самое место
//    ConstructTree(Read, list_compare);
//    ConstructTree(Write, list_compare);
//    if(_rebuild)
//    {

//        RecoveryTreeIndex();
//    }
}
//----------------------------------------------------------
void MainModule::BuildingTree(const QString& _user_login)
{
    //В таблицу CompareTablesToTree из локальной, ласт и серверной таблиц занести ориентируясь на Struct суммарные хеши моделей
    db_op->WriteToCompareTablesToTree(_user_login);

    //Провести анализ на направление изменений моделей в целом
    list_compare=db_op->AnalyzeCompareAll();

    //добавляем к стректуре моделей различающиеся файлы
    db_op->AddFilesToModelsStruct(list_compare);



}
//----------------------------------------------------------
void MainModule::RegisterUser(const qlonglong _s_num)
{
    //Получше напишу позже, это пока не так важно
    QString login="";//login_pass->GetLogin();
    QString password="";//login_pass->GetPassword();
    bool new_user=true;//login_pass->new_user;
    db_op->SaveLoginPassword(login, password, new_user, _s_num);

    //скрытие формы редактирования пользователя
    emit ShowEditLogin(false, false);
//    login_pass->setModal(false);
//    login_pass->setVisible(false);
//    login_pass->ClearAll();

    UpdateLogins();
}
//----------------------------------------------------------
void MainModule::DeleteUser(const qlonglong _s_num)
{
    db_op->DeleteLogin(_s_num);

    emit ShowEditLogin(false, false);
//    login_pass->setModal(false);
//    login_pass->setVisible(false);
//    login_pass->ClearAll();

    UpdateLogins();
}
//----------------------------------------------------------
void MainModule::UpdateLoginsTable(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateLogins(_block);

    UpdateLogins();

    StartAutoriz();
}
//----------------------------------------------------------
void MainModule::StartAutoriz()
{
    //показ формы авторизации
    tLog log;
    log.Write(tr("показ формы авторизации"));
    emit StartAutorizForm();
}
//----------------------------------------------------------
void MainModule::UpdateModelRead(QByteArray &_block)
{
    //распарсить переданый блок и записать в таблицу логинов
    db_op->UpdateModelRead(_block);

    emit VisibleMain(true);
//    this->setVisible(true);
}
//----------------------------------------------------------
void MainModule::InternalCallBuildingTree()
{
    if(!GetIsTransaction())
    {
        BuildingTree(_user_login);

        emit RebuildTrees(list_compare);
//        ConstructTree(Read, list_compare);
//        ConstructTree(Write, list_compare);
    }
}
//----------------------------------------------------------
