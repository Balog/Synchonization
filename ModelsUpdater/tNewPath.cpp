#include "tNewPath.h"
#include "ui_tNewPath.h"
#include<QDebug>

//extern tSettings my_settings;

tNewPath::tNewPath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tNewPath)
{
    ui->setupUi(this);
}
//-------------------------------------------------------
tNewPath::~tNewPath()
{
    delete ui;
}
//-------------------------------------------------------
void tNewPath::Visible(QString& _login, QString& _message, tExportMain* _main)
{
    main=_main;
    qDebug() << "tNewPath::Visible" << _login << _message;
    login=_login;
    ui->teMessage->setText(_message);
    this->setVisible(true);
}

//-------------------------------------------------------
void tNewPath::on_tbProjectFolder_clicked()
{
    project_folder=QFileDialog::getExistingDirectory(0, QString::fromUtf8(QString("Укажи рабочую папку проекта для пользователя '"+login+"'").toAscii()),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->leProjectFolder->setText(project_folder);
}
//-------------------------------------------------------
void tNewPath::on_tbTempFolder_clicked()
{
    temp_folder=QFileDialog::getExistingDirectory(0, QString::fromUtf8(QString("Укажи папку временных файлов проекта для пользователя '"+login+"'").toAscii()),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->leTempFolder->setText(temp_folder);
}
//-------------------------------------------------------
void tNewPath::on_pbOk_clicked()
{
    //нужно проверить наличие путей, из корректность и уникальность
    //а потом записать в базу данных и передать в главную форму
    QString pr_folder=ui->leProjectFolder->text();
    QString temp_folder=ui->leTempFolder->text();
    QString message="";

    bool ver=main->VerifyUserFolders(login, pr_folder, temp_folder, message);
    if(!ver)
    {
        ui->teMessage->setText(message);
    }
    else
    {
        //проверка пройдена
        //нужно записать в базу
        //и передать в глобальные установки

        main->SaveFoldersToLoginsTable(login, pr_folder, temp_folder);
        this->setVisible(false);
        ui->leProjectFolder->setText("");
        ui->leTempFolder->setText("");
        emit ContinueStart();
    }
}
//-------------------------------------------------------
void tNewPath::on_pbCancel_clicked()
{
        this->close();
}
