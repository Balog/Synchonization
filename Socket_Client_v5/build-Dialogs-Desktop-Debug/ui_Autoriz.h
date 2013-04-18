/********************************************************************************
** Form generated from reading UI file 'Autoriz.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTORIZ_H
#define UI_AUTORIZ_H

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

class Ui_Autoriz
{
public:
    QLineEdit *leLogin;
    QLineEdit *lePassword;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pbOk;

    void setupUi(QDialog *Autoriz)
    {
        if (Autoriz->objectName().isEmpty())
            Autoriz->setObjectName(QString::fromUtf8("Autoriz"));
        Autoriz->resize(291, 104);
        leLogin = new QLineEdit(Autoriz);
        leLogin->setObjectName(QString::fromUtf8("leLogin"));
        leLogin->setGeometry(QRect(60, 10, 221, 20));
        lePassword = new QLineEdit(Autoriz);
        lePassword->setObjectName(QString::fromUtf8("lePassword"));
        lePassword->setGeometry(QRect(60, 40, 221, 20));
        label = new QLabel(Autoriz);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 31, 16));
        label_2 = new QLabel(Autoriz);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 40, 41, 16));
        pbOk = new QPushButton(Autoriz);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));
        pbOk->setGeometry(QRect(110, 70, 75, 23));

        retranslateUi(Autoriz);
        QObject::connect(pbOk, SIGNAL(clicked()), Autoriz, SLOT(OnOk()));

        QMetaObject::connectSlotsByName(Autoriz);
    } // setupUi

    void retranslateUi(QDialog *Autoriz)
    {
        Autoriz->setWindowTitle(QApplication::translate("Autoriz", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Autoriz", "\320\233\320\276\320\263\320\270\320\275", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Autoriz", "\320\237\320\260\321\200\320\276\320\273\321\214", 0, QApplication::UnicodeUTF8));
        pbOk->setText(QApplication::translate("Autoriz", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Autoriz: public Ui_Autoriz {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTORIZ_H
