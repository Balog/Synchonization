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
    delete ui;
    delete main;
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

    connect(main, SIGNAL(Error(QString&)), this, SLOT(OnError(QString&)));

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
