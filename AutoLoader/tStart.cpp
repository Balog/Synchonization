#include "tStart.h"
#include "ui_tStart.h"
#include<QMessageBox>

tStart::tStart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tStart), main(NULL)
{
    ui->setupUi(this);


}
//-------------------------------------------------------
tStart::~tStart()
{
    delete main;
    delete ui;

}
//-------------------------------------------------------
void tStart::on_pushButton_clicked()
{
    //По нажатию на кнопку происходит полный цикл:
    //Подключение
    //Авторизация
    //Запрос файлов
    //Чтение
    //Отключение

    QTextCodec *codec =QTextCodec::codecForName("UTF-8");

    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);

    qDebug() << "Начало";
    main=new tAutoExportMain();

    connect(main, SIGNAL(FindServer(QString&, QString&)), this, SLOT(OnFindServer(QString&, QString&)));
    connect(main, SIGNAL(Error(QString&)), this, SLOT(OnError(QString&)));
    connect(main, SIGNAL(FoldersOk()),this, SLOT(OnFoldersOk()));
    connect(main, SIGNAL(SendModels(QList<tServerModel>&)), this, SLOT(OnSendModels(QList<tServerModel>&)));//
    connect(main, SIGNAL(EndTransactions()), this, SLOT(OnEndTransactions()));
}
//-------------------------------------------------------
void tStart::OnError(QString& error)
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8(error.toAscii()));
    MB.setWindowTitle(QString::fromUtf8("Ошибка"));
    MB.exec();

    delete main;
    main=NULL;
}
//-------------------------------------------------------
void tStart::OnFindServer(QString& _login, QString& _password)
{
    login=_login;
    password=_password;
    main->Autorization(login, password);
//    qDebug() << "Продолжаем работу, проверяем папки";
//    main->VerifyUserFolders();
}
//-------------------------------------------------------
void tStart::OnFoldersOk()
{
    qDebug() << "авторизовались, папки в порядке, продолжаем запуск";

    //список разрешенных к автообновлению моделей

    auto_models=main->ReadAutoUserModels();
    qDebug() << "Запрос списка моделей";
    //потом - запрос списка моделей
    main->OnListFiles();
}
//-------------------------------------------------------
void tStart::OnSendModels(QList<tServerModel> &_server_model)
{
    //Нужно проанализировать список моделей с сервера, пропускать те модели что не проходят по списку разрешенных
    //Потом нужно по моделям создавать транзакции чтения и запускать процесс
    FilterModelFiles(_server_model);
    main->ReceivingModels(new_models);
}
//-------------------------------------------------------
void tStart::FilterModelFiles(QList<tServerModel> &_server_model)
{
    for(int i=0; i<_server_model.size(); i++)
    {
        QString serv_struct=_server_model[i].Struct;

        for(int j=0; j<auto_models.size(); j++)
        {
            QString auto_struct=auto_models[j];
            if(auto_struct.right(1)=="\n")
            {
                auto_struct=auto_struct.left(auto_struct.length()-1);
            }
            if(serv_struct.left(auto_struct.length())==auto_struct)
            {
                //модель подходит

                tServerModel n_model;
                n_model.Description=_server_model[i].Description;
                n_model.DiskFile=_server_model[i].DiskFile;
                n_model.Struct=_server_model[i].Struct;
                n_model.Title=_server_model[i].Title;

                QList<tServerFile>n_files;
                for(int k=0; k<_server_model[i].Files.size(); k++)
                {
                    QFileInfo info(_server_model[i].Files[k].File);
                    QString path=info.path();
                    if(path.right(5)!=".info")
                    {
                        tServerFile n_file;
                        n_file.File=_server_model[i].Files[k].File;
                        n_file.Size=_server_model[i].Files[k].Size;
                        n_file.Hash_F=_server_model[i].Files[k].Hash_F;
                        n_file.LastMod_F=_server_model[i].Files[k].LastMod_F;
                        n_files.push_back(n_file);
                    }
                }

                n_model.Files=n_files;

                new_models.push_back(n_model);
                break;
            }
        }
    }
}
//-------------------------------------------------------
void tStart::OnEndTransactions()
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Готово!"));
    MB.setWindowTitle(QString::fromUtf8("Результат"));
    MB.exec();
}
