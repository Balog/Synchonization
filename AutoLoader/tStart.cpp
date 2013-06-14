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
    connect(main, SIGNAL(SendModels(QList<tServerModel>)), this, SLOT(OnSendModels(QList<tServerModel>)));

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



    //потом - запрос списка моделей
    main->OnListFiles();
}
//-------------------------------------------------------
void tStart::OnSendModels(QList<tServerModel> model)
{

}
