#ifndef TABSTRACTGUICOMMAND_H
#define TABSTRACTGUICOMMAND_H
#include <QDataStream>
#include <QtCore>
#include <QDialog>
//#include "ui_MainForm.h"
#include "tLog.h"


class tAbstractGuiCommand : public QObject
{
    Q_OBJECT
public:
    //Ui::MainForm *_ui
//    virtual void Initialize(){}
    virtual void ExeCommand(QDataStream& in) = 0;
    virtual void SetLink(QObject* _link) {link=_link;}

protected:

//    Ui::MainForm *ui;
    QObject* link;

    QString l;
    tLog log;


signals:
    void SendCommand(QByteArray block);
    void RunGui(QByteArray& block);
    void StartStop(bool res);
    void OkAutoriz(bool res);
    void NextCommand();
    void FinalBlockTransactions();
    void VerifyMoveDelete(QString&, bool);
    void EndConveyor(bool Ok);
    void EndCommand();
    void SendModels();

};

#endif // TABSTRACTGUICOMMAND_H
