#ifndef TABSTRACTGUICOMMAND_H
#define TABSTRACTGUICOMMAND_H
#include <QDataStream>
#include <QtCore>
#include <QDialog>
#include "ui_MainForm.h"



class tAbstractGuiCommand : public QObject
{
    Q_OBJECT
public:
    virtual void Initialize(Ui::MainForm *_ui){ui=_ui;}
    virtual void ExeCommand(QDataStream& in) = 0;
    virtual void SetLink(QObject* _link) {link=_link;}

protected:

    Ui::MainForm *ui;
//    MainForm *form;
QObject* link;

signals:
    void SendCommand(QByteArray block);
    void RunGui(QByteArray& block);
    void StartStop(bool res);
    void OkAutoriz(bool res);
    void NextCommand();
    void VerifyMoveDelete(QString&);
};

#endif // TABSTRACTGUICOMMAND_H
