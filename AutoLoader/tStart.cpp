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
    new_models=main->FilterModelFiles(_server_model, auto_models);
    main->ReceivingModels(new_models);
}

//-------------------------------------------------------
void tStart::OnEndTransactions()
{
    QMessageBox MB;
    MB.setText(QString::fromUtf8("Готово!"));
    MB.setWindowTitle(QString::fromUtf8("Результат"));
    MB.exec();

    main->ClearNewModel(new_models);
    delete main;
    main=NULL;
}

