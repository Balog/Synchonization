/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLineEdit *leAddres;
    QSpinBox *sbPort;
    QPushButton *pbConnect;
    QPushButton *pbDisconnect;
    QLineEdit *leCountClients;
    QPushButton *pbAddNew;
    QLineEdit *leRoot;
    QLabel *label;
    QLineEdit *leTemp;
    QLabel *label_2;
    QPushButton *pushButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(434, 300);
        leAddres = new QLineEdit(Dialog);
        leAddres->setObjectName(QString::fromUtf8("leAddres"));
        leAddres->setGeometry(QRect(10, 10, 121, 20));
        sbPort = new QSpinBox(Dialog);
        sbPort->setObjectName(QString::fromUtf8("sbPort"));
        sbPort->setGeometry(QRect(140, 10, 71, 22));
        sbPort->setMaximum(9999);
        sbPort->setValue(1234);
        pbConnect = new QPushButton(Dialog);
        pbConnect->setObjectName(QString::fromUtf8("pbConnect"));
        pbConnect->setGeometry(QRect(270, 10, 75, 23));
        pbDisconnect = new QPushButton(Dialog);
        pbDisconnect->setObjectName(QString::fromUtf8("pbDisconnect"));
        pbDisconnect->setGeometry(QRect(350, 10, 75, 23));
        leCountClients = new QLineEdit(Dialog);
        leCountClients->setObjectName(QString::fromUtf8("leCountClients"));
        leCountClients->setGeometry(QRect(214, 10, 51, 20));
        pbAddNew = new QPushButton(Dialog);
        pbAddNew->setObjectName(QString::fromUtf8("pbAddNew"));
        pbAddNew->setGeometry(QRect(10, 100, 101, 23));
        leRoot = new QLineEdit(Dialog);
        leRoot->setObjectName(QString::fromUtf8("leRoot"));
        leRoot->setGeometry(QRect(40, 40, 171, 20));
        leRoot->setReadOnly(false);
        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 43, 46, 13));
        leTemp = new QLineEdit(Dialog);
        leTemp->setObjectName(QString::fromUtf8("leTemp"));
        leTemp->setGeometry(QRect(40, 67, 171, 20));
        label_2 = new QLabel(Dialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 70, 46, 13));
        pushButton = new QPushButton(Dialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(120, 100, 91, 23));

        retranslateUi(Dialog);
        QObject::connect(pbConnect, SIGNAL(clicked()), Dialog, SLOT(OnConnect()));
        QObject::connect(pbDisconnect, SIGNAL(clicked()), Dialog, SLOT(OnDisconnect()));
        QObject::connect(pbAddNew, SIGNAL(clicked()), Dialog, SLOT(OnAddNew()));
        QObject::connect(pushButton, SIGNAL(clicked()), Dialog, SLOT(OnClearModels()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Server", 0, QApplication::UnicodeUTF8));
        leAddres->setText(QApplication::translate("Dialog", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        pbConnect->setText(QApplication::translate("Dialog", "Connect", 0, QApplication::UnicodeUTF8));
        pbDisconnect->setText(QApplication::translate("Dialog", "Disconnect", 0, QApplication::UnicodeUTF8));
        leCountClients->setText(QApplication::translate("Dialog", "0", 0, QApplication::UnicodeUTF8));
        pbAddNew->setText(QApplication::translate("Dialog", "\320\236\320\261\320\275\320\276\320\262\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "Root", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "Temp", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Dialog", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
