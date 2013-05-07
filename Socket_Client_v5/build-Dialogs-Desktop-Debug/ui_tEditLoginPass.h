/********************************************************************************
** Form generated from reading UI file 'tEditLoginPass.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEDITLOGINPASS_H
#define UI_TEDITLOGINPASS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_tEditLoginPass
{
public:
    QLineEdit *leLogin;
    QLineEdit *lePassword;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *tEditLoginPass)
    {
        if (tEditLoginPass->objectName().isEmpty())
            tEditLoginPass->setObjectName(QString::fromUtf8("tEditLoginPass"));
        tEditLoginPass->resize(362, 70);
        leLogin = new QLineEdit(tEditLoginPass);
        leLogin->setObjectName(QString::fromUtf8("leLogin"));
        leLogin->setGeometry(QRect(60, 10, 211, 20));
        lePassword = new QLineEdit(tEditLoginPass);
        lePassword->setObjectName(QString::fromUtf8("lePassword"));
        lePassword->setGeometry(QRect(60, 40, 211, 20));
        label = new QLabel(tEditLoginPass);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(7, 13, 46, 13));
        label_2 = new QLabel(tEditLoginPass);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 40, 46, 13));
        pushButton = new QPushButton(tEditLoginPass);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(280, 10, 75, 23));
        pushButton_2 = new QPushButton(tEditLoginPass);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(280, 38, 75, 23));

        retranslateUi(tEditLoginPass);
        QObject::connect(pushButton, SIGNAL(clicked()), tEditLoginPass, SLOT(OnSave()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), tEditLoginPass, SLOT(OnCancel()));

        QMetaObject::connectSlotsByName(tEditLoginPass);
    } // setupUi

    void retranslateUi(QDialog *tEditLoginPass)
    {
        tEditLoginPass->setWindowTitle(QApplication::translate("tEditLoginPass", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("tEditLoginPass", "\320\233\320\276\320\263\320\270\320\275", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("tEditLoginPass", "\320\237\320\260\321\200\320\276\320\273\321\214", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("tEditLoginPass", "\320\227\320\260\320\277\320\270\321\201\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("tEditLoginPass", "\320\236\321\202\320\274\320\265\320\275\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class tEditLoginPass: public Ui_tEditLoginPass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEDITLOGINPASS_H
